#ifndef FIELDD_H
#define FIELDD_H

using uint32 = std::uint32_t;

class Field3DD
{
private:

    void allocate()
    {
        data3D_ = new double** [nz_];
        data2D_ = new double* [nz_*ny_];
        data_   = new double [nx_*ny_*nz_];

        for (int i = 0; i < nz_; ++i )
        {
            data3D_[i] = data2D_ + i * ny_;
            for (int j = 0; j < ny_; ++j )
                data3D_[i][j] = data_ + (i*ny_ + j)* nz_;
        }
    }

public:

    Field3DD(uint32 nx, uint32 ny, uint32 nz)
    :nx_{nx}, ny_{ny}, nz_{nz}
    {
        allocate();
    }

    Field3DD(Field3DD const& source)
    :nx_{source.nx_}, ny_{source.ny_}, nz_{source.nz_}
    {
        allocate();
        for (auto i=0; i < nx_*ny_*nz_; ++i)
        {
            data_[i] = source.data_[i];
        }
    }


    inline double& operator()(uint32 const i, uint32 const j, uint32 const k)
    {
        return data3D_[i][j][k];
    }


    std::string name() {return "3 new";}

    std::array<std::size_t,3> shape() const {return std::array<std::size_t,3>{nx_, ny_, nz_};}

    ~Field3DD()
    {
        delete[] data3D_[0][0];
        delete[] data3D_[0];
        delete[] data3D_;
    }


private:
    uint32 nx_, ny_, nz_;
    double *data_;
    double **data2D_;
    double ***data3D_;
};

#endif
