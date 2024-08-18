#pragma once
#include <type/type.h>

#include <format>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace arte::transaction {
	struct Transaction {
		using id_type = std::size_t;
		id_type							 id{0};
		std::string						 name{""};
		std::vector<type::Type::id_type> ins;
		type::Type::id_type				 retT{0};
	};

	constexpr inline bool operator==(const Transaction& lhs,
									 const Transaction& rhs)
	{
		return lhs.id == rhs.id;
	}
	constexpr inline bool operator!=(const Transaction& lhs,
									 const Transaction& rhs)
	{
		return !operator==(lhs, rhs);
	}
	constexpr inline bool operator<(const Transaction& lhs,
									const Transaction& rhs)
	{
		return lhs.id < rhs.id;
	}

	struct Registry {
		void Add(Transaction::id_type id, std::string name,
				 std::vector<type::Type::id_type> ins, type::Type::id_type retT)
		{
			if (_registered.contains(id)) return;
			_registered.try_emplace(
				id, transaction::Transaction{
						.id = id, .name = name, .ins = ins, .retT = retT});
			_namesToId.try_emplace(name, id);
			_idToReturnType.try_emplace(id, retT);
		}

		std::optional<std::reference_wrapper<Transaction>>
		Find(const transaction::Transaction::id_type& transactionId)
		{
			if (!_registered.contains(transactionId)) { return {}; }
			return _registered.at(transactionId);
		}

		std::optional<std::reference_wrapper<Transaction>>
		Find(const std::string& transactionName)
		{
			if (!_namesToId.contains(transactionName)) { return {}; }
			return Find(_namesToId.at(transactionName));
		}

		std::unordered_map<transaction::Transaction::id_type,
						   transaction::Transaction>
			_registered;
		std::unordered_map<std::string, transaction::Transaction::id_type>
			_namesToId;
		std::unordered_map<transaction::Transaction::id_type,
						   type::Type::id_type>
			_idToReturnType;
	};
}  // namespace arte::transaction

namespace std {
	template <> struct hash<arte::transaction::Transaction> {
		constexpr inline std::size_t
		operator()(const arte::transaction::Transaction& obj) const
		{
			return obj.id;
		}
	};

	template <> struct formatter<arte::transaction::Transaction> {
		constexpr auto parse(std::format_parse_context& ctx)
		{
			return ctx.begin();
		}

		auto format(const arte::transaction::Transaction& transaction,
					std::format_context&				  ctx) const
		{
			return std::format_to(ctx.out(), "{} {}", transaction.id,
								  transaction.name);
		}
	};
}  // namespace std