#include "CppUnitTest.h"

#include <memory>
#include <string>
#include <stdexcept>
#include <format>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

///////////////////////////////////////////////////////////////////////////////

namespace clone
{

	///////////////////////////////////////////////////////////////////////////////

	class Interface
	{
	public:
		virtual ~Interface() {}

		virtual int foo() const = 0;
		virtual Interface* clone() const = 0;
	};

	///////////////////////////////////////////////////////////////////////////////

	class Impl_1 : public Interface
	{
	public:
		int foo() const override { return 1; }
		Interface* clone() const override { return new Impl_1{}; }
	};

	///////////////////////////////////////////////////////////////////////////////

	class Impl_2 : public Interface
	{
	public:
		int foo() const override { return 2; }
		Interface* clone() const override { return new Impl_2{}; }
	};

	///////////////////////////////////////////////////////////////////////////////

	class Wrapper
	{
		Interface* m_if;

	public:

		static Wrapper create(const std::string& impl_type)
		{
			if (impl_type == "impl 1") {
				return Wrapper{ std::make_unique<Impl_1>() };
			}
			else if (impl_type == "impl 2") {
				return Wrapper{ std::make_unique<Impl_2>() };
			}

			throw std::logic_error(std::format("Failed to create impl with type \"{}\"", impl_type));
		}

		explicit Wrapper(std::unique_ptr<Interface>&& if_)
			: m_if{ if_.release()}
		{}

		Wrapper(const Wrapper& other)
			: m_if{ other.m_if->clone() }
		{}

		Wrapper& operator=(Wrapper& other)
		{
			delete m_if;

			m_if = other.m_if->clone();

			return *this;
		}

		int foo() const { return m_if->foo(); }
	};

	///////////////////////////////////////////////////////////////////////////////
}

///////////////////////////////////////////////////////////////////////////////

namespace clone_test
{
	TEST_CLASS(Clone)
	{
	public:
		
		TEST_METHOD(CreateWrapperWithImpl1)
		{
			const auto wrapper = clone::Wrapper::create("impl 1");

			Assert::AreEqual(1, wrapper.foo());
		}

		TEST_METHOD(CreateWrapperWithImpl2)
		{
			const auto wrapper = clone::Wrapper::create("impl 2");

			Assert::AreEqual(2, wrapper.foo());
		}

		TEST_METHOD(CopyingPreservesImplType1)
		{
			const auto wrapper = clone::Wrapper::create("impl 1");

			const clone::Wrapper copy_of_wrapper(wrapper);

			Assert::AreEqual(1, copy_of_wrapper.foo());
		}

		TEST_METHOD(CopyingPreservesImplType2)
		{
			const auto wrapper = clone::Wrapper::create("impl 2");

			const clone::Wrapper copy_of_wrapper(wrapper);

			Assert::AreEqual(2, copy_of_wrapper.foo());
		}

		TEST_METHOD(AssignmentPreservesImplType1)
		{
			const auto wrapper = clone::Wrapper::create("impl 1");

			const auto copy_of_wrapper = wrapper;

			Assert::AreEqual(1, copy_of_wrapper.foo());
		}

		TEST_METHOD(AssignmentPreservesImplType2)
		{
			const auto wrapper = clone::Wrapper::create("impl 2");

			const auto copy_of_wrapper = wrapper;

			Assert::AreEqual(2, copy_of_wrapper.foo());
		}
	};
}

///////////////////////////////////////////////////////////////////////////////
