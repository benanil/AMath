# AMath
Optimized math library, that works with ARM NEON, SSE2-4.2 <br>
Also tested with GCC, CLANG, and MSVC, Android and Windows 10 platforms.

to use namespace 
```
#define AX_USE_NAMESPACE
// after this define you should use ax:: prefix
ax::Matrix4 mat;
```

To disable SIMD:<br>
if you don't want SIMD use this defines and compiler will generate scalar code instead of SIMD
```
#define AX_NO_SSE2
#define AX_NO_AVX2
```

# Math

The Math component of the ASTL library combines elements from glm and XNA Math<br>
while providing a simpler code structure that is easier to read, modify, and compile. <br>
The library offers various math structures to support common mathematical operations. Here are the available structures:<br>

* Vector2f, Vector2d, Vector2i, ...
* Vector3f, Vector3i, ...
* Vector4, ...
* Matrix4 (4x4 matrix)
* Matrix3 (3x3 matrix)
* Quaternion
* Transform
* Camera

The vectors are defined in the "Vector.hpp" header, while Matrix4 and Matrix3 are defined in the "Matrix.hpp" header.<br>
Other math structures have headers with the same name for easy access and organization.<br>

One notable feature of the ASTL math library is its simplicity and compactness.<br>
For example, the total lines of code for all vectors, including SIMD and non-SIMD versions,<br>
amount to only 440 lines. The Stack data structure consists of just 160 lines, and the QueueHeader totals 500 lines, including PriorityQueue.<br>
The Transform class encapsulates position, rotation, and scale properties, and it can be used with Euler angles as well.<br>
The Math.hpp header file contains all the essential math functions, including:<br>
<br>
Pow, Exp, Sqrt, Fract, Log, Log2, Log10, Floor, Sign, CopySign, Abs, FAbs, Min, Max<br>

Additionally, the library provides trigonometric functions such as:

Sin, Cos, Tan, ATan, Atan2, ASin, ACos <br>
Most of these functions are constexpr, and for those that are not,<br>
links are provided so that you can modify them to be constexpr as well.<br>
Although constexpr math functions are officially introduced in C++23, you can still use these functions with C++14.<br>
Example Usage of the Math Library:

```cpp
#include "Math/Vector.hpp"
#include "Math/Matrix.hpp"

static float f = 1.0f; f += 0.01f;
constexpr float distance = 3.14159265f; // this is distance from cube but I did use pi anyways 
Vector3f position(Sin(f) * distance, 0.0f, Cos(f) * distance );
float verticalFOV = 65.0f, nearClip = 0.01f, farClip = 500.0f;

Matrix4 projection = Matrix4::PerspectiveFovRH(verticalFOV * DegToRad, m_NativeWindow->GetWidth(), m_NativeWindow->GetHeight(), nearClip, farClip);
Matrix4 viewProjection = projection * Matrix4::LookAtRH(position, -Vector3f::Normalize(position), Vector3f::Up());

Vector3f baryCentrics = Vector3f(1.0f - hitOut.u - hitOut.v, hitOut.u, hitOut.v);
Matrix3 inverseMat3 = Matrix4::ConvertToMatrix3(hitInstance.inverseTransform);
		
Vector3f n0 = Matrix3::Multiply(inverseMat3, ConvertToFloat3(&triangle.normal0x));
Vector3f n1 = Matrix3::Multiply(inverseMat3, ConvertToFloat3(&triangle.normal1x));
Vector3f n2 = Matrix3::Multiply(inverseMat3, ConvertToFloat3(&triangle.normal2x));
	        
record.normal = Vector3f::Normalize((n0 * baryCentrics.x) + (n1 * baryCentrics.y) + (n2 * baryCentrics.z));

Vector2i x = Vector2i(1, 1) + Vector2(2, 2);
float length =  ToVector2f(x).Length();
```

The provided code demonstrates the usage of the math library.<br>
It showcases various operations involving vectors, matrices, and other math functions.<br>
Feel free to explore and utilize the math library based on your project's requirements.

here how it looks internaly, with bunch of links and comments that'll help you

```cpp
// https://mazzo.li/posts/vectorized-atan2.html
FINLINE constexpr float ATan(float x) {
	const float x_sq = x * x;
	constexpr float a1 =  0.99997726f, a3 = -0.33262347f, a5  = 0.19354346f,
	                a7 = -0.11643287f, a9 =  0.05265332f, a11 = -0.01172120f;
	return x * (a1 + x_sq * (a3 + x_sq * (a5 + x_sq * (a7 + x_sq * (a9 + x_sq * a11)))));
}

// https://en.wikipedia.org/wiki/Sine_and_cosine
// warning: accepts input between -TwoPi and TwoPi  if (Abs(x) > TwoPi) use x = FMod(x + PI, TwoPI) - PI;
FINLINE constexpr float Sin(float x) 
{
	float xx = x * x * x;                // x^3
	float t = x - (xx * 0.16666666666f); // x3/!3  6 = !3 = 1.6666 = rcp(3)
	t += (xx *= x * x) * 0.00833333333f; // x5/!5  120 = !5 = 0.0083 = rcp(5)
	t -= (xx *= x * x) * 0.00019841269f; // x7/!7  5040 = !7
	t += (xx * x * x)  * 2.75573e-06f;   // 362880 = !9
	return t;
}
```

Math library also has half to float, float to half conversion functions
and color packing and unpacking.
