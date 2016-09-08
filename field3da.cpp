// Example program
#include <iostream>
#include <vector>

class Field3DA
{
    private:
     double* data_;
     double ***data3D_;
     int nx_, ny_, nz_;


     public:
     Field3DA(int nx, int ny, int nz)
        :data_{nullptr}, data3D_{nullptr},
        nx_{nx},ny_{ny},nz_{nz}
     {
        data_ = new double[nx*ny*nz];
        data3D_ = new double**[nx];

        for (int i=0;  i < nx; ++i)
        {
            data3D_[i] = new double*[ny];
            for (int j=0; j < ny; ++j)
            {
                  data3D_[i][j] = data_ + i*nz*ny + j*nz;
            }
        }
     }// end Ctor


     double& operator()(int i, int j, int k)
     {
        return data3D_[i][j][k];
     }

     ~Field3DA()
     {
        for (int i=0; i < nx_; ++i)
        {
            delete [] data3D_[i]  ;
        }
         delete [] data3D_;
     }

};






int main()
{
  double dx=0.01, dy=0.02, dz=0.03;
  int nx=100, ny=1000, nz=1000;
  Field3DA fa{nx,ny,nz};

  for (int i=0; i < nx; ++i)
  {
        for (int j=0; j < ny; ++j)
        {
                for (int k=0; k < nz; ++k)
                {
                        fa(i,j,k) = dx*i + dy*j + dz*k;
                }
        }
  }



  std::cout << fa(10,12,20) << std::endl;

}













