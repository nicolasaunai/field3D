// Example program
#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>
#include <chrono>
#include <fstream>
#include <array>
#include <random>
#include <algorithm>
#include <cstdlib>

#include "fieldA.h"
#include "fieldB.h"

#ifdef XTENSOR
#include "fieldC.h"
#endif


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
    : field_{static_cast<uint32>(nx)+1,static_cast<uint32>(ny)+1,static_cast<uint32>(nz)+1}
    , gradFieldx_{static_cast<uint32>(nx),static_cast<uint32>(ny+1),static_cast<uint32>(nz+1)}
    , gradFieldy_{static_cast<uint32>(nx+1),static_cast<uint32>(ny),static_cast<uint32>(nz+1)}
    , gradFieldz_{static_cast<uint32>(nx+1),static_cast<uint32>(ny+1),static_cast<uint32>(nz)}
    , cellAvg_{static_cast<uint32>(nx),static_cast<uint32>(ny),static_cast<uint32>(nz)}
    , particles_(nbrParticles)
    , gen_(rd_())
    , xdistr_{0, nx}
    , ydistr_{0, ny}
    , zdistr_{0, nz}
    , repeatTimes_{repeatTimes}
    , readDurations_(repeatTimes)
    , writeDurations_(repeatTimes)
    , gradxDurations_(repeatTimes)
    , gradyDurations_(repeatTimes)
    , gradzDurations_(repeatTimes)
    , cellAvgDurations_(repeatTimes)
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



    double measureGradX()
    {
        int nx, ny, nz;
        auto shape = gradFieldx_.shape();

        nx = shape[0];
        ny = shape[1];
        nz = shape[2];
        double dx = 0.1;
        gradxDurations_.clear();

        for (int step = 0; step < repeatTimes_; ++step)
        {
            std::chrono::high_resolution_clock::time_point t1;
            t1 = std::chrono::high_resolution_clock::now();
            for (int i=0; i < nx; ++i)
            {
                for (int j=0; j < ny; ++j)
                {
                    for (int k=0; k < nz; ++k)
                    {
                        gradFieldx_(i,j,k) = dx*(field_(i+1,j,k) - field_(i,j,k));
                    }
                }
            }
            std::chrono::high_resolution_clock::time_point t2;
            t2 = std::chrono::high_resolution_clock::now();

            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1 ).count();
            gradxDurations_.push_back(duration);
        }
        return std::accumulate(std::begin(gradxDurations_), std::end(gradxDurations_), 0.0)/repeatTimes_;
    }


    double measureGradY()
    {
        int nx, ny, nz;
        auto shape = gradFieldy_.shape();
        nx = shape[0];
        ny = shape[1];
        nz = shape[2];
        double dx = 0.1;
        gradyDurations_.clear();

        for (int step = 0; step < repeatTimes_; ++step)
        {
            std::chrono::high_resolution_clock::time_point t1;
            t1 = std::chrono::high_resolution_clock::now();
            for (int i=0; i < nx; ++i)
            {
                for (int j=0; j < ny; ++j)
                {
                    for (int k=0; k < nz; ++k)
                    {
                        gradFieldy_(i,j,k) = dx*(field_(i,j+1,k) - field_(i,j,k));
                    }
                }
            }
            std::chrono::high_resolution_clock::time_point t2;
            t2 = std::chrono::high_resolution_clock::now();

            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1 ).count();
            gradyDurations_.push_back(duration);
        }
        return std::accumulate(std::begin(gradyDurations_), std::end(gradyDurations_), 0.0)/repeatTimes_;
    }



    double measureGradZ()
    {
        int nx, ny, nz;
        auto shape = gradFieldz_.shape();
        nx = shape[0];
        ny = shape[1];
        nz = shape[2];
        double dx = 0.1;
        gradzDurations_.clear();

        for (int step = 0; step < repeatTimes_; ++step)
        {
            std::chrono::high_resolution_clock::time_point t1;
            t1 = std::chrono::high_resolution_clock::now();
            for (int i=0; i < nx; ++i)
            {
                for (int j=0; j < ny; ++j)
                {
                    for (int k=0; k < nz; ++k)
                    {
                        gradFieldz_(i,j,k) = dx*(field_(i,j,k+1) - field_(i,j,k));
                    }
                }
            }
            std::chrono::high_resolution_clock::time_point t2;
            t2 = std::chrono::high_resolution_clock::now();

            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1 ).count();
            gradzDurations_.push_back(duration);
        }
        return std::accumulate(std::begin(gradzDurations_), std::end(gradzDurations_), 0.0)/repeatTimes_;
    }


    double measureAvg()
    {
        int nx, ny, nz;
        auto shape = cellAvg_.shape();
        nx = shape[0];
        ny = shape[1];
        nz = shape[2];
        double dx = 0.1;
        cellAvgDurations_.clear();
        double oneEighth = 1./6.;

        for (int step = 0; step < repeatTimes_; ++step)
        {
            std::chrono::high_resolution_clock::time_point t1;
            t1 = std::chrono::high_resolution_clock::now();
            for (int i=0; i < nx; ++i)
            {
                for (int j=0; j < ny; ++j)
                {
                    for (int k=0; k < nz; ++k)
                    {
                        cellAvg_(i,j,k) = oneEighth*( field_(i,j,k)
                                                    + field_(i+1,j,k)
                                                    + field_(i,j+1,k)
                                                    + field_(i+1,j+1,k)
                                                    + field_(i,j,k+1)
                                                    + field_(i+1,j,k+1)
                                                    + field_(i,j+1,k)
                                                    + field_(i,j+1,k+1)
                                                    );
                    }
                }
            }
            std::chrono::high_resolution_clock::time_point t2;
            t2 = std::chrono::high_resolution_clock::now();

            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1 ).count();
            cellAvgDurations_.push_back(duration);
        }
        return std::accumulate(std::begin(cellAvgDurations_), std::end(cellAvgDurations_), 0.0)/repeatTimes_;
    }


    void analyze()
    {
        auto readTime  = measureRead();
        auto writeTime = measureWrite();
        auto gradxTime  = measureGradX();
        auto gradyTime  = measureGradY();
        auto gradzTime  = measureGradZ();
        auto avgTime    = measureAvg();
        std::cout << "random read access time  : " << readTime << "ms " <<  readTime/1e6 << "sec\n";
        std::cout << "random write access time : " << writeTime << "ms " <<  writeTime/1e6 << "sec\n";
        std::cout << "gradx time               : "  << gradxTime << "ms " <<  gradxTime/1e6 << "sec\n";
        std::cout << "grady time               : "  << gradyTime << "ms " <<  gradyTime/1e6 << "sec\n";
        std::cout << "gradz time               : "  << gradzTime << "ms " <<  gradzTime/1e6 << "sec\n";
        std::cout << "average time             : "  << avgTime << "ms " <<  avgTime/1e6 << "sec\n";
        std::cout << "-----------------------------------------------------\n";
    }


    void show(std::ostream& output)
    {
        //output << std::fixed;
        for (auto readDuration : readDurations_)
        {
            output << readDuration << " ";
        }
        output << "\n";

        for (auto writeDuration  : writeDurations_)
        {
            output << writeDuration << " ";
        }
        output << "\n";

        for (auto gradxDuration : gradxDurations_)
        {
            output << gradxDuration << " ";
        }
        output << "\n";

        for (auto gradyDuration : gradyDurations_)
        {
            output << gradyDuration << " ";
        }
        output << "\n";

        for (auto gradzDuration : gradzDurations_)
        {
            output << gradzDuration << " ";
        }
        output << "\n";

        for (auto avgDuration : cellAvgDurations_)
        {
            output << avgDuration << " ";
        }
        output << "\n";
    }

private:

    Field field_;
    Field gradFieldx_;
    Field gradFieldy_;
    Field gradFieldz_;
    Field cellAvg_;

    std::vector<Particle> particles_;
    std::random_device rd_;
    std::mt19937 gen_;
    std::uniform_int_distribution<> xdistr_, ydistr_, zdistr_;
    int repeatTimes_;
    std::vector<double> readDurations_;
    std::vector<double> writeDurations_;
    std::vector<double> gradxDurations_;
    std::vector<double> gradyDurations_;
    std::vector<double> gradzDurations_;
    std::vector<double> cellAvgDurations_;
    double tmpTotal_;

};






int main(int argc, char **argv)
{
  int nx, ny, nz;
  nx = std::atoi(argv[1]);
  ny = std::atoi(argv[2]);
  nz = std::atoi(argv[3]);

  //int nx= 20, ny=20, nz=20;
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
     std::ofstream file{"A.txt"};
     analyzer.show(file);
 }

 std::cout << "\n";

 {
      PerfAnalyzer<Field3DB> analyzer{nx,ny,nz,repeatTimes, nbrParticles};
      analyzer.analyze();
      std::ofstream file{"B.txt"};
      analyzer.show(file);
 }

 std::cout << "\n";

#ifdef XTENSOR
 {
      PerfAnalyzer<Field3DC> analyzer{nx,ny,nz,repeatTimes, nbrParticles};
      analyzer.analyze();
      std::ofstream file{"C.txt"};
      analyzer.show(file);
 }
#endif


}
