#ifndef ACTIVATION_SOFTMAX_H_
#define ACTIVATION_SOFTMAX_H_

namespace MiniDNN
{


///
/// \ingroup Activations
///
/// The softmax activation function
///
class Softmax
{
    private:
        typedef Eigen::Matrix<Scalar, Eigen::Dynamic, Eigen::Dynamic> Matrix;
        typedef Eigen::Array<Scalar, 1, Eigen::Dynamic> RowArray;

    public:
        // a = activation(z) = softmax(z)
        // Z = [z1, ..., zn], A = [a1, ..., an], n observations
        static inline void activate(const Matrix& Z, Matrix& A)
        {
		//	printf("Softmax Z: ");
		//	for (int i=0; i<Z.size(); ++i)
		//	{
		//		printf("%.3f, ", Z.data()[i]);
		//	}
		//	printf("\n");

		//	Matrix t = (Z.rowwise() - Z.colwise().maxCoeff()).array();

			// WangLiang: 如果 gap > 100.0f 那么经过 exp，A.array() 中就会有 0.0f
			// 导致后级 MultiClassEntropy.h 中 evaluate 计算 1/x 得到 NaN
		//	Scalar gap = (Z.colwise().maxCoeff() - Z.colwise().minCoeff()).maxCoeff();
		//	printf("Softmax activate max gap: %.2f\n", gap);

            A.array() = (Z.rowwise() - Z.colwise().maxCoeff()).array().exp();
            RowArray colsums = A.colwise().sum();
            A.array().rowwise() /= colsums;

		//	for (int i=0; i<A.size(); ++i)
		//		assert(A.data()[i]!=0.0f);

		//	printf("Softmax A: ");
		//	for (int i=0; i<A.size(); ++i)
		//	{
		//		printf("%.3f, ", A.data()[i]);
		//	}
		//	printf("\n");
        }

        // Apply the Jacobian matrix J to a vector f
        // J = d_a / d_z = diag(a) - a * a'
        // g = J * f = a .* f - a * (a' * f) = a .* (f - a'f)
        // Z = [z1, ..., zn], G = [g1, ..., gn], F = [f1, ..., fn]
        // Note: When entering this function, Z and G may point to the same matrix
        static inline void apply_jacobian(const Matrix& Z, const Matrix& A,
                                          const Matrix& F, Matrix& G)
        {
		//	for (int i=0; i<Z.size(); ++i)
		//		assert(!isnan(Z.data()[i]));
		//	for (int i=0; i<A.size(); ++i)
		//		assert(!isnan(A.data()[i]));
		//	for (int i=0; i<F.size(); ++i)
		//		assert(!isnan(F.data()[i]));

            RowArray a_dot_f = A.cwiseProduct(F).colwise().sum();
            G.array() = A.array() * (F.array().rowwise() - a_dot_f);

		//	for (int i=0; i<G.size(); ++i)
		//		assert(!isnan(G.data()[i]));
        }

        static std::string return_type()
        {
            return "Softmax";
        }
};


} // namespace MiniDNN


#endif /* ACTIVATION_SOFTMAX_H_ */
