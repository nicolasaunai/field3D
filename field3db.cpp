// Example program
#include <iostream>
#include <vector>
#include <cstdint>
#include <chrono>
#include <fstream>
#include <array>
#include <random>
#include <algorithm>

#include "fieldA.h"
#include "fieldB.h"
#include "fieldC.h"

using uint32 = std::uint32_t;





template <typename Field>
void init(Field& f)
{
    std::size_t nx, ny, nz;
    auto shape = f.shape();
    nx = shape[0];
    ny = shape[1];
    nz = shape[2];

    for (std::size_t i=0; i < nx; ++i)
    {
        for ( uint32 j=0; j < ny; ++j)
        {
            for (uint32 k=0; k < nz; ++k)
            {
                f(i,j,k) = static_cast<double>(i + j + k);
            }
        }
    }// end array loop
}



struct Particle
{
    uint32 i,j,k;
};



template <typename Field>
class PerfAnalyzer
{

public:

    void initParticles()
    {
        for (auto& particle : particles_)
        {
            particle.i = xdistr_(gen_);
            particle.j = ydistr_(gen_);
            particle.k = zdistr_(gen_);
        }
    }

    PerfAnalyzer(int nx, int ny, int  nz,
                 int repeatTimes, int nbrParticles=2000000)
    : field_{static_cast<uint32>(nx),static_cast<uint32>(ny),static_cast<uint32>(nz)}
    , particles_(nbrParticles)
    , gen_(rd_())
    , xdistr_{0, nx-1}
    , ydistr_{0, ny-1}
    , zdistr_{0, nz-1}
    , repeatTimes_{repeatTimes}
    , readDurations_(repeatTimes)
    , writeDurations_(repeatTimes)
    {
        std::cout << field_.name() << "\n";
        initParticles();
        init(field_);
    }

    double measureWrite()
    {
        writeDurations_.clear();
        for (int step = 0; step < repeatTimes_; ++step)
        {
            //std::shuffle(std::begin(particles_), std::end(particles_), gen_);

            std::chrono::high_resolution_clock::time_point t1;
            t1 = std::chrono::high_resolution_clock::now();

            for (auto const& particle : particles_)
            {
                field_(particle.i, particle.j, particle.k) = particle.i + particle.k;
            }

            std::chrono::high_resolution_clock::time_point t2;
            t2 = std::chrono::high_resolution_clock::now();

            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1 ).count();
            writeDurations_.push_back(duration);
        }

        return std::accumulate(std::begin(writeDurations_), std::end(writeDurations_), 0.0)/repeatTimes_;
    }


    double measureRead()
    {
        readDurations_.clear();
        std::vector<double> measurements;
        for (int step = 0; step < repeatTimes_; ++step)
        {
            //std::shuffle(std::begin(particles_), std::end(particles_), gen_);

            std::chrono::high_resolution_clock::time_point t1;
            t1 = std::chrono::high_resolution_clock::now();

            tmpTotal_ = 0.;
            for (auto const& particle : particles_)
            {
                tmpTotal_ += field_(particle.i, particle.j, particle.k);
            }

            std::chrono::high_resolution_clock::time_point t2;
            t2 = std::chrono::high_resolution_clock::now();

            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1 ).count();
            readDurations_.push_back(duration);
        }
        return std::accumulate(std::begin(readDurations_), std::end(readDurations_), 0.0)/repeatTimes_;
    }


    void analyze()
    {
        auto readTime  = measureRead();
        auto writeTime = measureWrite();
        std::cout << readTime << "ms " <<  readTime/1e6 << "sec\n";
        std::cout << writeTime << "ms " <<  writeTime/1e6 << "sec\n";
        std::cout << "-----------------------------------------------------\n";
    }

private:

    Field field_;
    std::vector<Particle> particles_;
    std::random_device rd_;
    std::mt19937 gen_;
    std::uniform_int_distribution<> xdistr_, ydistr_, zdistr_;
    int repeatTimes_;
    std::vector<double> readDurations_;
    std::vector<double> writeDurations_;
    double tmpTotal_;

};






int main()
{
  int nx= 20, ny=20, nz=20;
  int nbrParticlesPerCell = 100;
  int nbrParticles = nx*ny*nz*nbrParticlesPerCell;
  int repeatTimes = 1000;

  std::cout << "-----------------------------------------------------\n";
  std::cout << "Number of cells              : " << nx << ", " << ny << ", " << nz << "\n";
  std::cout << "Number of particles per cell : " << nbrParticlesPerCell << "\n";
  std::cout << "Experiment repeated          : " << repeatTimes << " times\n";
  std::cout << "-----------------------------------------------------\n\n\n";

 {
     PerfAnalyzer<Field3DA> analyzer{nx,ny,nz,repeatTimes, nbrParticles};
     analyzer.analyze();

 }

 std::cout << "\n";

 {
      PerfAnalyzer<Field3DB> analyzer{nx,ny,nz,repeatTimes, nbrParticles};
      analyzer.analyze();
 }

 std::cout << "\n";

 {
      PerfAnalyzer<Field3DC> analyzer{nx,ny,nz,repeatTimes, nbrParticles};
      analyzer.analyze();
 }



}
