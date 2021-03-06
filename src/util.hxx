
/*
Copyright (c) 2014, Florian Sittel (www.lettis.net)
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met: 

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer. 
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution. 

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "util.hpp"
#include "errors.hpp"

#include <string>
#include <cmath>
#include <algorithm>

namespace FastPCA {

  namespace {
    template <class T>
    void
    _elementwise_mult(Matrix<T>& m, T factor) {
      std::size_t nr = m.n_rows();
      std::size_t nc = m.n_cols();
      std::size_t i,j;
      for (j=0; j < nc; ++j) {
        for (i=0; i < nr; ++i) {
          m(i,j) *= factor;
        }
      }
    }

    template <class T>
    T
    _sign(T v) {
      return (v < 0) ? -1.0 : 1.0;
    }
  } // end local namespace

  template <class T>
  std::vector<std::size_t>
  sorted_index(const std::vector<T> &v, bool reverse_sorting) {
    std::vector<std::size_t> indeces(v.size());
    for (std::size_t i = 0; i != v.size(); ++i) {
      indeces[i] = i;
    }
    if (reverse_sorting) {
      std::sort(indeces.begin(), indeces.end(), [&v](std::size_t i1, std::size_t i2) {return v[i1] > v[i2];});
    } else {
      std::sort(indeces.begin(), indeces.end(), [&v](std::size_t i1, std::size_t i2) {return v[i1] < v[i2];});
    }
    return indeces;
  }

  template <class T>
  void
  deg2rad_inplace(Matrix<T>& m) {
    _elementwise_mult(m, M_PI / 180.0);
  }

  template <class T>
  void
  deg2rad_inplace(std::vector<T>& v) {
    for (std::size_t i=0; i < v.size(); ++i) {
      v[i] *= M_PI / 180.0;
    }
  }
  
  template <class T>
  void
  rad2deg_inplace(Matrix<T>& m) {
    _elementwise_mult(m, 180.0 / M_PI);
  }
  
  template <class T>
  void
  shift_matrix_columns_inplace(Matrix<T>& m, std::vector<T> shifts) {
    std::size_t i,j;
    const std::size_t n_rows = m.n_rows();
    const std::size_t n_cols = m.n_cols();
    #pragma omp parallel for default(none)\
                             private(i,j)\
                             firstprivate(n_rows,n_cols)\
                             shared(m,shifts)
    for (j=0; j < n_cols; ++j) {
      for (i=0; i < n_rows; ++i) {
        m(i,j) = m(i,j) - shifts[j];
      }
    }
  }

  template <class T>
  void
  scale_matrix_columns_inplace(Matrix<T>& m, std::vector<T> factors) {
    std::size_t i,j;
    const std::size_t n_rows = m.n_rows();
    const std::size_t n_cols = m.n_cols();
    #pragma omp parallel for default(none)\
                             private(i,j)\
                             firstprivate(n_rows,n_cols)\
                             shared(m,factors)
    for (j=0; j < n_cols; ++j) {
      for (i=0; i < n_rows; ++i) {
        m(i,j) = m(i,j) * factors[j];
      }
    }
  }

  template <class T>
  std::vector<T> parse_line(std::string line) {
    std::vector<T> out;
    std::size_t len = line.length();
    const char* last_start = &line[0];
    std::size_t j=0;
    bool whitespace_before = true;
    while (j < len) {
      if (line[j] == ' ') {
        if ( ! whitespace_before) {
          line[j] = '\0';
          out.push_back(atof(last_start));
          whitespace_before = true;
        }
      } else {
        // now we have some character which is no whitespace
        if (whitespace_before) {
          last_start = &line[j];
        }
        whitespace_before = false;
      }
      ++j;
    }
    out.push_back(atof(last_start));
    return out;
  }

  namespace Periodic {
    template <class T>
    T
    normalized(T var, T periodicity) {
      periodicity *= 0.5;
      T var1 = std::fmod(var,   periodicity);
      T var2 = std::fmod(var, 2*periodicity);
      return (var1 == var2) ? var1 : (var1 - _sign(var2-var1) * periodicity);
    }

    template <class T>
    void
    shift_matrix_columns_inplace(Matrix<T>& m
                               , std::vector<T> shifts
                               , std::vector<T> periodicities) {
      std::size_t i,j;
      const std::size_t n_rows = m.n_rows();
      const std::size_t n_cols = m.n_cols();
      #pragma omp parallel for default(none)\
                               private(i,j)\
                               firstprivate(n_rows,n_cols)\
                               shared(m,shifts, periodicities)
      for (j=0; j < n_cols; ++j) {
        for (i=0; i < n_rows; ++i) {
          m(i,j) = m(i,j) - shifts[j];
          // periodic boundary checks
          m(i,j) = FastPCA::Periodic::normalized(m(i,j), periodicities[j]);
        }
      }
    }

    template <class T>
    void
    shift_matrix_columns_inplace(Matrix<T>& m
                               , std::vector<T> shifts) {
      std::vector<T> p(shifts.size(), 2*M_PI);
      FastPCA::Periodic::shift_matrix_columns_inplace(m, shifts, p);
    }
  } // FastPCA::Periodic

} // namespace FastPCA

