// Example program
#include <iostream>
#include <vector>




class Field3DB
{
    private:
    int nx_, ny_, nz_;
    //std::vector < double > data_;
    double *data_;

    public:

    Field3DB(int nx, int ny, int nz)
    :nx_{nx}, ny_{ny}, nz_{nz}//,data_(nx*ny*nz)
    {
        data_ = new double [nx*ny*nz];
    }

    double& operator()(int i, int j, int k)
    {
        return data_[k + j*nz_ + i*ny_*nz_];
    }
    ~Field3DB(){delete [] data_;}
};




int main()
{
  double dx=0.01, dy=0.02, dz=0.03;
  int nx=100, ny=1000, nz=1000;
  Field3DB fb{nx,ny,nz};

  for (int i=0; i < nx; ++i)
  {
        for (int j=0; j < ny; ++j)
        {
                for (int k=0; k < nz; ++k)
                {
                        fb(i,j,k) = dx*i + dy*j + dz*k;
                }
        }
  }
  std::cout << fb(10,12,20) << std::endl;
}













