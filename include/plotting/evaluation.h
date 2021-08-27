#ifndef EVALUATION_H
#define EVALUATION_H

#include <typeindex>

// Runs the parse tree
enum ResultType {
	INTEGER, FLOAT
};

class Any {
    std::type_index type;
    std::unique_ptr<void> value;
  public:
    template<typename T>
    Any(const T& rhs): type(typeid(T)), value(std::make_unique<void>(rhs)){}

    template<typename T>
    Any& operator=(const T& rhs){
        type = typeid(T);
        value = std::make_unique<void>(rhs);
    }
    template<typename T>
    T get(){
        return *static_cast<T*>(value.get());
    }
    int getInt(){
        return *static_cast<int*>(value.get());
    }
    float getFloat(){
        return *static_cast<float*>(value.get());
    }
};

struct InterpretResult {
    double     value;
    ResultType type;
};

template<typename... A>
struct InterpretFunction {
	short variableCount = 1;
    void* func;

    InterpretFunction(short variableCount, double (*func)(A...)):
		variableCount(variableCount),
		func(reinterpret_cast<void*>(func)){}

	template<typename... ArgsT>
	double operator()(ArgsT&& ... args){
        return reinterpret_cast<double (*)(A...)>(func)(std::forward<ArgsT>(args)...);
	}
};

std::ostream& operator<<(std::ostream& os, const InterpretResult& result){
    if(result.type == FLOAT)       os << static_cast<float>(result.value);
    else if(result.type == INTEGER) os << static_cast<int>(result.value);
    return os;
}

static std::unordered_map<std::string, std::variant<InterpretResult, InterpretFunction<double>>> hashTable = {
	{"pi", InterpretResult{M_PI, ResultType::FLOAT}},
    {"e", InterpretResult{M_E, ResultType::FLOAT}},
    {"sin", InterpretFunction<double>(1, sin)},
    {"cos", InterpretFunction<double>(1, cos)},
    {"tan", InterpretFunction<double>(1, tan)}
};

//struct InterpretFunction {
//	int variableCount = 1;
//    InterpretResult (*pFunction)(InterpretResult);
//};
//static std::unordered_map<std::string,

InterpretResult interpret(AST::Node& node);

InterpretResult interpretArguments(AST::Node& node) {
    auto lhs = interpret(*node.left());
    return lhs;
}

InterpretResult unary(AST::Node& node) {
    auto lhs = interpret(*node.left());
    double value;
    switch (node.value.type) {
        case TOKEN_PLUS: value = lhs.value;     break;
        case TOKEN_MIN: value  = -lhs.value;    break;
    }
	// Very simple way of determining if a number is a float or int
    ResultType type = value - static_cast<int>(value) != 0 ? FLOAT : INTEGER;
    return {value, type};
}


InterpretResult binary(AST::Node& node) {
    auto lhs = interpret(*node.left());
    auto rhs = interpret(*node.right());
    double value;
    switch (node.value.type) {
        case TOKEN_POW: value = pow(lhs.value, rhs.value);  break;
        case TOKEN_PLUS: value  = lhs.value + rhs.value;    break;
        case TOKEN_MIN: value   = lhs.value - rhs.value;    break;
        case TOKEN_MUL: value   = lhs.value * rhs.value;    break;
        case TOKEN_SLASH: value = lhs.value / rhs.value;    break;
    }
    ResultType type = value - static_cast<int>(value) != 0 ? FLOAT : INTEGER;
    return {value, type};
}

bool isUnary(AST::Node& ast){
	return ast.right()->value.start == nullptr;
}

// helper constant for the visitor #3
template<class> inline constexpr bool always_false_v = false;

// helper type for the visitor #4
template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
// explicit deduction guide (not needed as of C++20)
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;



InterpretResult interpret(AST::Node& ast) {
    auto value = ast.value;
    switch(value.type){
//	case TOKEN_ASSIGNMENT:
	case TOKEN_IDENTIFIER: {
		std::string identifier = std::string(value.start, value.length);
		auto        it         = hashTable.find(identifier);

		if(it != hashTable.end()) {
			if(std::holds_alternative<InterpretResult>(it->second)){
                auto result = std::get<InterpretResult>(it->second);
				return result;
			} else if(std::holds_alternative<InterpretFunction<double>>(it->second)){
                auto result = std::get<InterpretFunction<double>>(it->second);
				auto args = interpretArguments(ast);
				return InterpretResult{result(args.value), ResultType::FLOAT};
			}
		} else {
			// Error
			std::cerr << "Unknown identifier \"" << identifier << "\"." << std::endl;
		}
		break;
	}
    case TOKEN_PLUS:
    case TOKEN_MIN:
        if(isUnary(ast)){
			return unary(ast);
		}
    case TOKEN_POW:
    case TOKEN_MUL:
    case TOKEN_SLASH:  return binary(ast);

    case TOKEN_INTEGER: return {static_cast<double>(std::stoi(std::string(value.start, value.length))), INTEGER};
    case TOKEN_FLOAT:  return {std::stod(std::string(value.start, value.length)), FLOAT};
    case TOKEN_EOF:    break;
    default: throw;
    }
}


#endif //EVALUATION_H
