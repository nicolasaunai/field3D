// Example program
#include <iostream>
#include <vector>
#include <cstdint>
#include <chrono>
#include <fstream>
#include <array>
#include <random>
#include <algorithm>

using uint32 = std::uint32_t;


// ---------------------------------------------------------------------------
class Field3DA
{
private:

     double* data_;
     double ***data3D_;
     uint32 nx_, ny_, nz_;

public:

     void allocate()
     {
         data_ = new double[nx_ * ny_ * nz_];
         data3D_ = new double**[nx_];


         for (uint32 i=0;  i < nx_; ++i)
         {
             data3D_[i] = new double*[ny_];
             if (data3D_[i] == nullptr)
             {
                 throw std::runtime_error("Error");
             }
             for (uint32 j=0; j < ny_; ++j)
             {
                   data3D_[i][j] = data_ + i * nz_ * ny_ + j * nz_;
             }
         }
     }


     Field3DA(uint32 nx, uint32 ny, uint32 nz)
        :data_{nullptr}, data3D_{nullptr},
        nx_{nx},ny_{ny},nz_{nz}
     {
         allocate();
     }


     Field3DA(Field3DA const& source)
     : data_{nullptr}, data3D_{nullptr}
     , nx_{source.nx_}, ny_{source.ny_}, nz_{source.nz_}
     {
         allocate();
         for (auto i=0; i < nx_*ny_*nz_; ++i)
         {
             data_[i] = source.data_[i];
         }
     }


     std::array<std::size_t,3> shape() const {return std::array<std::size_t,3>{nx_, ny_, nz_};}


     double& operator()(uint32 i, uint32 j, uint32 k)
     {
        return data3D_[i][j][k];
     }

     ~Field3DA()
     {
         for (uint32 i=0; i < nx_; ++i)
         {
             if (data3D_[i])
                delete [] data3D_[i];
            else
                std::cout << "no need " << i << "\n";
         }
         delete [] data3D_;
     }
};
// ---------------------------------------------------------------------------




// ---------------------------------------------------------------------------
class Field3DB
{
    public:

    Field3DB(uint32 nx, uint32 ny, uint32 nz)
    :nx_{nx}, ny_{ny}, nz_{nz}, nynz{ny*nz},data_(nx*ny*nz) {}

    double& operator()(uint32 i, uint32 j, uint32 k)
    {
        return data_[ k + j*nz_ + i*nynz ];
    }

    std::array<std::size_t,3> shape() const {return std::array<std::size_t,3>{nx_, ny_, nz_};}

    ~Field3DB() = default;


private:
    uint32 nx_, ny_, nz_;
    uint32 nynz;
    std::vector <double> data_;
};
// ---------------------------------------------------------------------------




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
        std::cout << "initializing particles...\n";
        initParticles();
        std::cout << "initializing field...\n";
        init(field_);
        std::cout << "initialization done...\n";
    }

    double measureWrite()
    {
        writeDurations_.clear();
        for (int step = 0; step < repeatTimes_; ++step)
        {
            std::shuffle(std::begin(particles_), std::end(particles_), gen_);

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
            std::shuffle(std::begin(particles_), std::end(particles_), gen_);

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
  int nbrParticles = nx*ny*nz*100;
  int repeatTimes = 100;


 {
     std::cout << "Multiple allocations" << std::endl;
     PerfAnalyzer<Field3DB> analyzer{nx,ny,nz,repeatTimes, nbrParticles};
     auto readTime  = analyzer.measureRead();
     auto writeTime = analyzer.measureWrite();
     std::cout << readTime << "ms " <<  readTime/1e6 << "sec\n";
     std::cout << writeTime << "ms " <<  writeTime/1e6 << "sec\n";
 }

 std::cout << "\n";

 {
      std::cout << "Contiguous" << std::endl;
      PerfAnalyzer<Field3DB> analyzer{nx,ny,nz,repeatTimes, nbrParticles};
      auto readTime  = analyzer.measureRead();
      auto writeTime = analyzer.measureWrite();
      std::cout << readTime << "ms " <<  readTime/1e6 << "sec\n";
      std::cout << writeTime << "ms " <<  writeTime/1e6 << "sec\n";
 }

}
