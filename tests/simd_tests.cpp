#include <c4/simd.hpp>

#include <array>
#include <vector>
#include <iostream>

#include <random>

using namespace std;
using namespace c4::simd;

// ===================================================== HELPERS ================================================================

template<class T>
T random() {
    static std::mt19937 mt;
    static std::uniform_int_distribution<long long> d;
    
    return (T)d(mt);
}

template<>
float random<float>() {
    static std::mt19937 mt;
    static std::uniform_real_distribution<float> d;

    return d(mt);
}

template<class T, int n>
array<T, n> random_array() {
    array<T, n> v;
    for (T& t : v)
        t = random<T>();
    return v;
}

template<class T>
T all_ones() {
    T t;
    memset(&t, -1, sizeof(t));
    return t;
}

class Exception : public std::runtime_error {
public:
    Exception(std::string msg, std::string filename, int line) : runtime_error(msg + " at " + filename + ":" + std::to_string(line)) {}
};

#define THROW_EXCEPTION(MSG) throw Exception(MSG, __FILE__, __LINE__)

#define ASSERT_TRUE(C) if( C ) {} else THROW_EXCEPTION("Runtime assertion failed: " #C)
#define ASSERT_EQUAL(A, B) if( (A) == (B) ) {} else THROW_EXCEPTION("Runtime assertion failed: " #A " == " #B ", " + to_string(A) + " != " + to_string(B))

// ====================================================== TESTS =================================================================

template<class T>
void test_cmpgt() {
    constexpr int n = 16 / sizeof(T);
    auto a = random_array<T, n>();
    auto b = random_array<T, n>();
    auto r = random_array<T, n>();

    auto va = load(a.data());
    auto vb = load(b.data());
    auto vr = cmpgt(va, vb);

    store(r.data(), vr);

    for (int i = 0; i < n; i++) {
        ASSERT_EQUAL(r[i], a[i] > b[i] ? all_ones<T>() : 0);
    }
}

void multitest_cmpgt() {
    test_cmpgt<int8_t>();
    test_cmpgt<uint8_t>();
    test_cmpgt<int16_t>();
    test_cmpgt<uint16_t>();
    test_cmpgt<int32_t>();
    test_cmpgt<uint32_t>();
}

template<class T>
void test_min() {
    constexpr int n = 16 / sizeof(T);
    auto a = random_array<T, n>();
    auto b = random_array<T, n>();
    auto r = random_array<T, n>();

    auto va = load(a.data());
    auto vb = load(b.data());
    auto vr = min(va, vb);

    store(r.data(), vr);

    for (int i = 0; i < n; i++) {
        ASSERT_EQUAL(r[i], min(a[i], b[i]));
    }
}

void multitest_min() {
    test_min<int8_t>();
    test_min<uint8_t>();
    test_min<int16_t>();
    test_min<uint16_t>();
    test_min<int32_t>();
    test_min<uint32_t>();
    test_min<float>();
}

template<class T>
void test_max() {
    constexpr int n = 16 / sizeof(T);
    auto a = random_array<T, n>();
    auto b = random_array<T, n>();
    auto r = random_array<T, n>();

    auto va = load(a.data());
    auto vb = load(b.data());
    auto vr = max(va, vb);

    store(r.data(), vr);

    for (int i = 0; i < n; i++) {
        ASSERT_EQUAL(r[i], max(a[i], b[i]));
    }
}

void multitest_max() {
    test_max<int8_t>();
    test_max<uint8_t>();
    test_max<int16_t>();
    test_max<uint16_t>();
    test_max<int32_t>();
    test_max<uint32_t>();
    test_max<float>();
}

template<class T>
void test_interleave() {
    constexpr int n = 16 / sizeof(T);
    auto a = random_array<T, 2 * n>();
    auto r = random_array<T, 2 * n>();

    auto va = load(a.data());
    auto vb = load(a.data() + n);
    c4::simd::tuple<decltype(va), 2> vavb{ va, vb };
    auto vr = interleave(vavb);

    store(r.data(), vr.val[0]);
    store(r.data() + n, vr.val[1]);

    for (int i = 0; i < 2 * n; i++) {
        ASSERT_EQUAL(r[i], i % 2 ? a[n + i / 2] : a[i/2]);
    }
}

void multitest_interleave() {
    test_interleave<int8_t>();
    test_interleave<uint8_t>();
    test_interleave<int16_t>();
    test_interleave<uint16_t>();
    test_interleave<int32_t>();
    test_interleave<uint32_t>();
    test_interleave<float>();
}

template<class T>
void test_deinterleave() {
    constexpr int n = 16 / sizeof(T);
    auto a = random_array<T, 2 * n>();
    auto r = random_array<T, 2 * n>();

    auto va = load(a.data());
    auto vb = load(a.data() + n);
    c4::simd::tuple<decltype(va), 2> vavb{ va, vb };
    auto vr = deinterleave(vavb);

    store(r.data(), vr.val[0]);
    store(r.data() + n, vr.val[1]);

    for (int i = 0; i < 2 * n; i++) {
        ASSERT_EQUAL(a[i], i % 2 ? r[n + i / 2] : r[i / 2]);
    }
}

void multitest_deinterleave() {
    test_deinterleave<int8_t>();
    test_deinterleave<uint8_t>();
    test_deinterleave<int16_t>();
    test_deinterleave<uint16_t>();
    test_deinterleave<int32_t>();
    test_deinterleave<uint32_t>();
    test_deinterleave<float>();
}

// ======================================================= MAIN =================================================================

int main()
{
    try {
        const int n_steps = 1000;

        for (int k = 0; k < n_steps; k++) {
            multitest_cmpgt();
            multitest_min();
            multitest_max();
            multitest_interleave();
            multitest_deinterleave();
        }
    }
    catch (std::exception& e) {
        cout << e.what() << endl;
    }

    return 0;
}

