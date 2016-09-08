// Example program
#include <iostream>
#include <vector>
#include <cstdint>
#include <chrono>
#include <fstream>


using uint32 = std::uint32_t;

class Field3DA
{
    private:
     double* data_;
     double ***data3D_;
     uint32 nx_, ny_, nz_;


     public:
     Field3DA(uint32 nx, uint32 ny, uint32 nz)
        :data_{nullptr}, data3D_{nullptr},
        nx_{nx},ny_{ny},nz_{nz}
     {
        data_ = new double[nx*ny*nz];
        data3D_ = new double**[nx];

        for (uint32 i=0;  i < nx; ++i)
        {
            data3D_[i] = new double*[ny];
            for (uint32 j=0; j < ny; ++j)
            {
                  data3D_[i][j] = data_ + i*nz*ny + j*nz;
            }
        }
     }// end Ctor


     double& operator()(uint32 i, uint32 j, uint32 k)
     {
        return data3D_[i][j][k];
     }

     ~Field3DA()
     {
        for (uint32 i=0; i < nx_; ++i)
        {
            delete [] data3D_[i]  ;
        }
         delete [] data3D_;
     }

};






int main()
{

  double dx=0.01, dy=0.02, dz=0.03;
  uint32 nx=1000, ny=1000, nz=1000;
  int ntot=2000000;
  Field3DA fa{nx,ny,nz};
  int repeatTimes = 10;
  std::vector<double> assignTimes(repeatTimes);
  std::vector<double> summationTimes(repeatTimes);
  std::vector<double> totStore(repeatTimes);

    std::cout << "SMILEI-like" << std::endl;


#if 0
  for (uint32 i=0; i < nx; ++i)
  {
        for ( uint32 j=0; j < ny; ++j)
        {
                for (uint32 k=0; k < nz; ++k)
                {
                        fb(i,j,k) = dx*i + dy*j + dz*k;
                }
        }
  }
#endif


    std::vector<uint32> ii(ntot);
    std::vector<uint32> jj(ntot);
    std::vector<uint32> kk(ntot);

    for (int m=0; m <repeatTimes; ++m)
    {
        for (int n=0; n < ntot; ++n)
        {
            ii[n] = (rand() % (uint32)(nx));
            jj[n] = (rand() % (uint32)(ny));
            kk[n] = (rand() % (uint32)(nz));
        }

    std::cout << "iteration " << m << std::endl;

    std::chrono::high_resolution_clock::time_point t1;
    t1 = std::chrono::high_resolution_clock::now();

    for (int n=0; n < ntot; ++n)
    {
        fa(ii[n],jj[n],kk[n]) = ii[n]+jj[n]+kk[n];
    }

  std::chrono::high_resolution_clock::time_point t2;
  t2 = std::chrono::high_resolution_clock::now();

  auto duration = std::chrono::duration_cast<std::chrono::microseconds>( t2 - t1 ).count();

 // std::cout << "assignment time (run " << m <<" ) = " 
  //    << duration/1000000. << "sec" << std::endl;

  assignTimes[m] = static_cast<double>(duration/1000000.);

    double tot=0.;


    t1 = std::chrono::high_resolution_clock::now();


    for (int n=0; n < ntot; ++n)
    {
        tot+=fa(ii[n],jj[n],kk[n]);
    }

   t2 = std::chrono::high_resolution_clock::now();

    duration = std::chrono::duration_cast<std::chrono::microseconds>( t2 - t1 ).count();

 // std::cout << "summation time (run " << m <<" ) = " << duration/1000000. << "sec" << std::endl;
  summationTimes[m] = static_cast<double>(duration/1000000.);
   totStore[m] = tot;

    }



    std::ofstream f{"results_A.txt"};
    double mean1=0., mean2=0.;
    for (int m=0; m < repeatTimes; ++m)
    {
        std::cout << "( "<< assignTimes[m]
                  << ", "<< summationTimes[m] << ") tot = " << totStore[m] << std::endl;
        f  <<  m << " " <<  assignTimes[m]
                  << "  "<< summationTimes[m]  << std::endl;
        mean1+=assignTimes[m];
        mean2+=summationTimes[m];
    }
    std::cout << "mean(assignment) = " << mean1/static_cast<double>(repeatTimes) << std::endl;
    std::cout << "mean(summation)  = " << mean2/static_cast<double>(repeatTimes) << std::endl;

}













