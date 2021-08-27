#include "glm/glm.hpp"
#include "ml/ml.h"

#include <benchmark/benchmark.h>
#include <complex>
#include <math-lib2/include/ml/ml.h>

template<typename T, size_t N>
struct vector {
    static_assert(N != 0, "vector must contain at least one element");
    T data[N];
	constexpr void set(size_t i, T value) {
		data[i] = value;
	}

    constexpr T& operator[](size_t i) noexcept { return data[i]; }
    constexpr const T &operator[](size_t i) const noexcept { return data[i]; }

    constexpr vector<T, N> operator*(const vector<T, N>& rhs) const;
    bool operator<=>(vector<T, N> vec){

	}

};

template<typename T, size_t N>
constexpr vector<T, N> vector<T,N>::operator*(const vector<T, N>& rhs) const {
    vector<T, N> result{};
    for(int i = 0; i < N; i++){
        result[i] = this->operator[](i) * rhs[i];
    }
    return result;
}
template <typename... Args> constexpr decltype(auto) make_vector(Args... args) {
    return vector{args...};
}
template <typename T, typename... U>
vector(T, U...)->vector<std::enable_if_t<(std::is_same_v<T, U> && ...), T>,1 + sizeof...(U)>;

template <typename T, std::size_t N> vector(const T (&)[N])->vector<T, N>;


static void BM_(benchmark::State& state) {
	for(auto _: state) {
        constexpr vector<float, 3> asd {1,2,3};
		constexpr vector<float, 3> bsd = asd * asd;
		static_assert(bsd[1] == 4);
	}
}

static void BM2_(benchmark::State& state) {
    for(auto _: state) {
        constexpr glm::vec3 asd {1,2,3};
		constexpr glm::vec3 bsd = asd * asd;
        static_assert(bsd[1] == 4);
    }

}

static void BM3_(benchmark::State& state) {
    for(auto _: state) {
        constexpr ml::vec3 asd {1.0f,2.0f,3.0f};
        constexpr ml::vec3 bsd = asd * asd;
        static_assert(bsd[1] == 4);
    }

}



BENCHMARK(BM_);
BENCHMARK(BM2_);
BENCHMARK(BM3_);

BENCHMARK_MAIN();
