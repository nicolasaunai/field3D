#ifndef FIELDC_H
#define FIELDC_H

#include "xtensor/xarray.hpp"

using uint32 = std::uint32_t;

class Field3DC
{
    public:

    Field3DC(uint32 nx, uint32 ny, uint32 nz)
    :nx_{nx}, ny_{ny}, nz_{nz}, nynz{ny*nz},data_{std::vector<std::size_t>{nx,ny,nz}} {}

    double& operator()(uint32 i, uint32 j, uint32 k)
    {
        return data_(i,j,k);
    }

    std::string name() {return "xtensor";}

    std::array<std::size_t,3> shape() const {return std::array<std::size_t,3>{nx_, ny_, nz_};}

    ~Field3DC() = default;


private:
    uint32 nx_, ny_, nz_;
    uint32 nynz;
    xt::xarray<double> data_;
};

#endif
