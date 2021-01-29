#include "flyft/field.h"
#include "flyft/fourier_transform.h"
#include "flyft/mesh.h"
#include "flyft/rosenfeld_fmt.h"
#include "flyft/state.h"

#include <iostream>
#include <memory>
#include <vector>

int main()
    {
    auto mesh = std::make_shared<flyft::Mesh>(10.0,50);
    auto state = std::make_shared<flyft::State>(mesh,1);

    // testing FFT
    const int N = 6;
    auto fft = std::make_shared<flyft::FourierTransform>(N, 1.0);
    std::vector<double> vec(N);
    for (int i=0; i < N; ++i)
        {
        vec[i] = i+1;
        }
    fft->setData(vec.data(), flyft::FourierTransform::RealSpace);
    for (int i=0; i < N; ++i)
        {
        std::cout << " " << fft->getData().get()[i];
        }
    std::cout << std::endl;
    fft->transform();
    for (int i=0; i < 2*(N/2+1); ++i)
        {
        std::cout << " " << fft->getData().get()[i];
        }
    std::cout << std::endl;
    fft->transform();
    for (int i=0; i < N; ++i)
        {
        std::cout << " " << fft->getData().get()[i];
        }
    std::cout << std::endl;

    return 0;
    }
