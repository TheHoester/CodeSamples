#pragma once

namespace Engine
{
	/**
	 * Singleton
	 * When inherited by other classes, it will turn them into a singleton class,
	 * allowing for only one object of the class to exist as any one time.
	 */
	template <typename T>
	class Singleton
	{
	protected:
		Singleton() { }
		~Singleton(void) { }

	public:
		/**
		 * Instance()
		 * Provides a singleton instance of the class.
		 * Will create the instance upon first calling.
		 * @return The instance of the class.
		 */
		static T& Instance()
		{
			static T instance;
			return instance;
		}

		Singleton(Singleton const&) = delete;
		void operator=(Singleton const&) = delete;
	};
}