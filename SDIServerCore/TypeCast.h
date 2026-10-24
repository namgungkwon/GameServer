#pragma once

#include "Types.h"


///////////////////////////////////////////////////////
//	<	TypeList	>
// TypeList<Type1, Type2>::Head -> Type1
// TypeList<Type1, Type2>::Tail -> Type2
// TypeLIst<Type1, TypeList<Type2, Type3>>::Tail::Head -> Type2
// TypeLIst<Type1, TypeList<Type2, Type3>>::Tail::Tail -> Type3
///////////////////////////////////////////////////////
#pragma region TypeList
template<typename... T>
struct TypeList;

template<typename T, typename U>
struct TypeList<T, U>
{
	using Head = T;
	using Tail = U;
};

template<typename T, typename... U>
struct TypeList<T, U...>
{
	using Head = T;
	using Tail = TypeList<U...>;
};
#pragma endregion	// TypeList



///////////////////////////////////////////////////////
//	<	Length	>
// Length<TypeList<Type1, Type2>>::value -> 2
// Length<TypeList<Type1, Type2, Type3>>::value -> 3
///////////////////////////////////////////////////////
#pragma region Length
template<typename T>
struct Length;

template<>
struct Length<TypeList<>>
{
	enum { value = 0 };
};

template<typename T, typename... U>
struct Length<TypeList<T, U...>>
{
	enum { value = 1 + Length<TypeList<U...>>::value };
};
#pragma endregion



///////////////////////////////////////////////////////
//	<	TypeAt	>
// TypeAt<TypeList<Type1, Type2, Type3>, 0> -> Type1
// TypeAt<TypeList<Type1, Type2, Type3>, 2> -> Type2
// TypeAt<TypeList<Type1, Type2, Type3>, 3> -> Type3
///////////////////////////////////////////////////////
#pragma region TypeAt
template<typename TL, int32 index>
struct TypeAt;

template<typename Head, typename... Tail>
struct TypeAt<TypeList<Head, Tail...>, 0>
{
	using Result = Head;
};

template<typename Head, typename... Tail, int32 index>
struct TypeAt<TypeList<Head, Tail...>, index>
{
	using Result = typename TypeAt<TypeList<Tail...>, index - 1>::Result;
};
#pragma endregion



///////////////////////////////////////////////////////
//	<	IndexOf		>
// IndefOf<TypeList<Type1, Type2>, Type2>> -> 2
// IndefOf<TypeList<Type1, Type2>, Type10>> -> -1
///////////////////////////////////////////////////////
#pragma  region IndexOf
template<typename TL, typename T>
struct IndexOf;

template<typename... Tail, typename T>
struct IndexOf<TypeList<T, Tail...>, T>
{
	enum { value = 0 };
};

template<typename T>
struct IndexOf<TypeList<>, T>
{
	enum { value = -1 };
};

template<typename Head, typename... Tail, typename T>
struct IndexOf<TypeList<Head, Tail...>, T>
{
private:
	enum { temp = IndexOf<TypeList<Tail...>, T>::value };

public:
	enum { value = (temp == -1) ? -1 : temp + 1 };
};
#pragma endregion



///////////////////////////////////////////////////////
//	<	Conversion	>
// if) Type2 :: Type1
// Conversion<Type1, Type2>::exists -> 0
// Conversion<Type2, Type1>::exists -> 1
// Conversion<Type1, Type3>::exists -> 0
///////////////////////////////////////////////////////
#pragma region Conversion
template<typename From, typename To>
class Conversion
{
private:
	using Small = __int8;
	using Big = __int32;

	static Small Test(const To&) { return 0; }
	static Big Test(...) { return 0; }
	static From MakeFrom() { return 0; }

public:
	enum
	{
		exists = sizeof(Test(MakeFrom())) == sizeof(Small)
	};
};

#pragma endregion



///////////////////////////////////////////////////////
//	<	TypeCast	>
// bool canCast = CanCast<Type2*>(type1);
// Type2 type2 = TypeCast<Type2*>(type1);
///////////////////////////////////////////////////////
#pragma region TypeCast
template<int32 v>
struct Int2Type
{
	enum { value = v };
};

template<typename TL>
class TypeConversion
{
public:
	enum
	{
		length = Length<TL>::value
	};

	TypeConversion()
	{
		MakeTable(Int2Type<0>(), Int2Type<0>());
	}

	template<int32 i, int32 j>
	static void MakeTable(Int2Type<i>, Int2Type<j>)
	{
		using FromType = typename TypeAt<TL, i>::Result;
		using ToType = typename TypeAt<TL, j>::Result;

		if (Conversion<const FromType*, const ToType*>::exists)
			s_convert[i][j] = true;
		else
			s_convert[i][j] = false;

		MakeTable(Int2Type<i>(), Int2Type<j + 1>());
	}

	template<int32 i>
	static void MakeTable(Int2Type<i>, Int2Type<length>)
	{
		MakeTable(Int2Type<i + 1>(), Int2Type<0>());
	}

	template<int j>
	static void MakeTable(Int2Type<length>, Int2Type<j>)
	{}

	static inline bool CanConvert(int32 from, int32 to)
	{
		static TypeConversion conversion;
		return s_convert[from][to];
	}

public:
	static bool s_convert[length][length];
};

template<typename TL>
bool TypeConversion<TL>::s_convert[length][length];

template<typename To, typename From>
To TypeCast(From* ptr)
{
	if (ptr == nullptr)
		return nullptr;

	using TL = typename From::TL;

	if (TypeConversion<TL>::CanConvert(ptr->_typeId, IndexOf<TL, remove_pointer_t<To>>::value))
		return static_cast<To>(ptr);

	return nullptr;
}


template<typename To, typename From>
shared_ptr<To> TypeCast(shared_ptr<From> ptr)
{
	if (ptr == nullptr)
		return nullptr;

	using TL = typename From::TL;

	if (TypeConversion<TL>::CanConvert(ptr->_typeId, IndexOf<TL, remove_pointer_t<To>>::value))
		return static_pointer_cast<To>(ptr);

	return nullptr;
}

template<typename To, typename From>
bool CanCast(From* ptr)
{
	if (ptr == nullptr)
		return false;

	using TL = typename From::TL;
	return TypeConversion<TL>::CanConvert(ptr->_typeId, IndexOf<TL, remove_pointer_t<To>>::value);
}

template<typename To, typename From>
bool CanCast(shared_ptr<From> ptr)
{
	if (ptr == nullptr)
		return false;

	using TL = typename From::TL;
	return TypeConversion<TL>::CanConvert(ptr->_typeId, IndexOf<TL, remove_pointer_t<To>>::value);
}
#pragma endregion



// using TL = TypeList<class Type1, class type2, class Type3, class Type4>
// 이런식으로 전역에 선언을 해주어야 한다.
#define DECLARE_TL		using TL = TL; int32 _typeId; // 클래스 내부에서 상단에서 선언
#define INIT_TL(Type)	_typeId = IndexOf<TL, Type>::value; // 생성자에서 선언