// Example program
#include <iostream>
#include <vector>
#include <cstdint>
#include <chrono>
#include <fstream>

using uint32 = std::uint32_t;


class Field3DB
{
    private:
    uint32 nx_, ny_, nz_;
    uint32 nynz;
    //std::vector < double > data_;

    public:
    double *data_;

    Field3DB(uint32 nx, uint32 ny, uint32 nz)
    :nx_{nx}, ny_{ny}, nz_{nz}, nynz{ny*nz}//,data_(nx*ny*nz)
    {
        data_ = new double [nx*ny*nz];
    }

    double& operator()(uint32 i, uint32 j, uint32 k)
    {
        return data_ [ k + j*nz_ + i*nynz ];
    }


    ~Field3DB(){delete [] data_;}
};




int main()
{
  double dx=0.01, dy=0.02, dz=0.03;
  uint32 nx=1000, ny=1000, nz=1000;
  int ntot=2000000;
  Field3DB fb{nx,ny,nz};
  int repeatTimes = 10;
  std::vector<double> assignTimes(repeatTimes);
  std::vector<double> summationTimes(repeatTimes);
  std::vector<double> totStore(repeatTimes);

    std::cout << "PHARE-like" << std::endl;

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
            ii[n] = (rand() % (int)(nx));
            jj[n] = (rand() % (int)(ny));
            kk[n] = (rand() % (int)(nz));
        }


    std::cout << "iteration " << m << std::endl;

    std::chrono::high_resolution_clock::time_point t1;
    t1 = std::chrono::high_resolution_clock::now();

    for (int n=0; n < ntot; ++n)
    {
        fb(ii[n],jj[n],kk[n]) = ii[n]+jj[n]+kk[n];
    }

  std::chrono::high_resolution_clock::time_point t2;
  t2 = std::chrono::high_resolution_clock::now();

  auto duration = std::chrono::duration_cast<std::chrono::microseconds>( t2 - t1 ).count();

  //std::cout << "assignment time (run " << m <<" ) = "
  //    << duration/1000000. << "sec" << std::endl;

  assignTimes[m] = static_cast<double>(duration/1000000.);


    double tot=0.;


    t1 = std::chrono::high_resolution_clock::now();


    for (int n=0; n < ntot; ++n)
    {
        tot+=fb(ii[n],jj[n],kk[n]);
    }

   t2 = std::chrono::high_resolution_clock::now();

    duration = std::chrono::duration_cast<std::chrono::microseconds>( t2 - t1 ).count();

//std::cout << "summation time (run " << m <<" ) = " << duration/1000000. << "sec" << std::endl;
  summationTimes[m] = static_cast<double>(duration/1000000.);
 //std::cout << "tot = " << tot << std::endl;
   totStore[m] = tot;

    }

    std::ofstream f{"results_B.txt"};
    double mean1=0., mean2=0.;
    for (int m=0; m < repeatTimes; ++m)
    {
        std::cout << "( "<< assignTimes[m]
                  << ", "<< summationTimes[m] << ") tot = " << totStore[m]<< std::endl;
        f  <<  m << " " <<  assignTimes[m]
                  << " "<< summationTimes[m]  << std::endl;
        mean1+=assignTimes[m];
        mean2+=summationTimes[m];
    }

    std::cout << "mean(assignment) = " << mean1/static_cast<double>(repeatTimes) << std::endl;
    std::cout << "mean(summation)  = " << mean2/static_cast<double>(repeatTimes) << std::endl;
}













