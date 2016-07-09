
#ifndef __PRIMITIVES_H__
#define __PRIMITIVES_H__

namespace renderer
{
	class Primitives
	{
		public:
			/// default constructor
			Primitives();
			/// default destructor
			~Primitives();

			/// InitialisePrimitives - loads the shaders for rendering primitives
			void InitialisePrimitives();
			/// ShutdownPrimitives - releases the shaders for rendering primitives
			void ShutdownPrimitives();

			/// DrawSphere - draws a sphere
			/// \param radius - radius of the sphere
			void DrawSphere(float radius, const glm::vec4& colour);

			/// DrawLine - draws a line
			/// \param start - start point of the line
			/// \param end - end point of the line
			/// \param colour - colour of the colour
			void DrawLine(const glm::vec3 &start, const glm::vec3 &end, const glm::vec4& colour);

		private:
			renderer::Shader m_PrimitiveShader;
			GLint m_ModelViewProjectionMatrix;
			GLint m_VertexColour;
			GLint m_VertexAttribLocation;

	};

} // namespace renderer

#endif // __PRIMITIVES_H__
