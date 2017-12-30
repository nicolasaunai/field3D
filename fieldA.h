#ifndef FIELDA_H
#define FIELDA_H

using uint32 = std::uint32_t;

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

#endif
