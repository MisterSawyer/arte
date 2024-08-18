#pragma once
#include <format>
#include <string>
#include <unordered_map>
#include <unordered_set>

namespace arte::type {
	struct Type {
		using id_type = std::size_t;
		id_type		id{0};
		std::string name{""};
	};

	constexpr inline bool operator==(const Type& lhs, const Type& rhs)
	{
		return lhs.id == rhs.id;
	}
	constexpr inline bool operator!=(const Type& lhs, const Type& rhs)
	{
		return !operator==(lhs, rhs);
	}
	constexpr inline bool operator<(const Type& lhs, const Type& rhs)
	{
		return lhs.id < rhs.id;
	}

	class Engine {
	  public:
		std::unordered_map<type::Type::id_type, type::Type> _registered;
		// std::unordered_map<type::Type::id_type,
		// std::unordered_map<std::string, CPyObject>> _resultSpace;
	};
}  // namespace arte::type

namespace std {
	template <> struct hash<arte::type::Type> {
		constexpr inline std::size_t
		operator()(const arte::type::Type& obj) const
		{
			return obj.id;
		}
	};

	template <> struct formatter<arte::type::Type> {
		constexpr auto parse(std::format_parse_context& ctx)
		{
			return ctx.begin();
		}

		auto format(const arte::type::Type& type,
					std::format_context&	ctx) const
		{
			return std::format_to(ctx.out(), "{} {}", type.id, type.name);
		}
	};
}  // namespace std