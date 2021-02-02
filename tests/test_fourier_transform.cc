#include "flyft/fourier_transform.h"

#include <iostream>
#include <vector>

int main()
    {
    // testing FFT
    const int N = 6;
    flyft::FourierTransform fft(N);
    std::vector<double> vec(N);
    for (int i=0; i < N; ++i)
        {
        vec[i] = i+1;
        }
    fft.setRealData(vec.data());
    for (int i=0; i < fft.getRealSize(); ++i)
        {
        std::cout << " " << fft.getRealData()[i];
        }
    std::cout << std::endl;
    fft.transform();
    for (int i=0; i < fft.getReciprocalSize(); ++i)
        {
        std::cout << " " << fft.getReciprocalData()[i];
        }
    std::cout << std::endl;
    fft.transform();
    for (int i=0; i < fft.getRealSize(); ++i)
        {
        std::cout << " " << fft.getRealData()[i];
        }
    std::cout << std::endl;

    return 0;
    }
