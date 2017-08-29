/*******************************************************************************
    Author: Alexey Frolov (alexwin32@mail.ru)

    This software is distributed freely under the terms of the MIT License.
    See "LICENSE" or "http://copyfree.org/content/standard/licenses/mit/license.txt".
*******************************************************************************/

#pragma once
#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <algorithm>
#include <sstream>
#include <memory.h>
#include <Exception.h>
#include <Vector2.h>
#include <Utils/ToString.h>

DECLARE_EXCEPTION(FormatException);
DECLARE_EXCEPTION(ArraySizeException);
DECLARE_EXCEPTION(NumberConvertionError);

template<class TVar>
TVar ParseNumber(const std::wstring &String)
{
    std::wistringstream iss(String);

    TVar val;
    iss >> std::noskipws >> val;

    if(!iss.eof() || iss.fail())
        throw NumberConvertionError(String + L" is not a number");

    return val;
}

template<class TVar>
TVar ParseNumber(const std::string &String)
{
    std::istringstream iss(String);

    TVar val;
    iss >> std::noskipws >> val;

    if(!iss.eof() || iss.fail())
        throw NumberConvertionError(String + " is not a number");

    return val;
}

template<class TVar, class RandFunc>
TVar GetRandomFromRange(const std::string &FullString, RandFunc RandFunction) throw (FormatException)
{
    auto vals = SerializerTools<StringParser>::PairFromString(FullString, '<', '>');

    TVar minVal = ParseNumber<TVar>(vals.first.c_str()), maxVal = ParseNumber<TVar>(vals.second.c_str());

    if(minVal > maxVal) 
        throw FormatException("invalid values for string " + FullString);

    return RandFunction(minVal, maxVal);
}

template <class Parser, class ParseFunc>
typename Parser::ValueType ParseValue(const std::string &FullString, ParseFunc ParseFunction) throw (FormatException)
{
    std::string values = FullString;
    values.erase(remove_if(values.begin(), values.end(), isspace), values.end());

    RangeI params = GetStatementParameters(values, '{', '}');
    if(params.minVal == -1 && params.maxVal == -1)
        return ParseFunction(values);

    std::vector<typename Parser::ValueType> arr = SerializerTools<Parser>::ArrayFromString(values, '{', '}', ',');
    return arr[rand() % arr.size()];
}

template<class Func>
void ParseString(const std::string &String, Func Operator) throw (Exception)
{
    std::map<char, char> specCharsMapping;

    std::vector<char> specCharsLog;

    specCharsMapping[']'] = '[';
    specCharsMapping[')'] = '(';
    specCharsMapping['}'] = '{';
    specCharsMapping['>'] = '<';

    int strInd = 0;

    for(char c : String){

        if(c == '[' || c == '(' || c == '{' || c == '<')
            specCharsLog.push_back(c);

        if(c == ']' || c == ')' || c == '}' || c == '>'){

            if(specCharsLog.size() == 0 || specCharsMapping[c] != specCharsLog.back())
                throw FormatException(Utils::ToString("unexpected character in '", c, "' in string ", String, ". Cymbol ", (strInd + 1)));

            specCharsLog.pop_back();
        }

        Operator(c, specCharsLog, strInd++);
    }

    if(specCharsLog.size())
        throw FormatException("invalid string " + String);
};

inline RangeI GetStatementParameters(const std::string &String, char OpenSymbol, char CloseSymbol)
{
    RangeI params = {-1, -1};

    ParseString(String, [&](char Char, const std::vector<char> &SpecCharsLog, int StrInd)
    {
        if(Char == OpenSymbol && SpecCharsLog.size() == 1)
            params.minVal = StrInd;
        else if(Char == CloseSymbol && SpecCharsLog.size() == 0)
            params.maxVal = StrInd;
    });

    return params;
}

template<typename Parser>
class SerializerTools
{
public:
	typedef std::pair<typename Parser::ValueType, typename Parser::ValueType> Pair;	
	static Pair PairFromString(const std::string &String, char OpenSymbol, char CloseSymbol) throw (FormatException)
	{
		RangeI params = GetStatementParameters(String, OpenSymbol, CloseSymbol);
		
		if(params.minVal == -1 || params.maxVal == -1) throw FormatException("invalid string " + String);
			
		bool isSecondVal = false;
		std::string firstVal = "", secondVal = "";

		std::string values = String.substr(params.minVal + 1, params.maxVal - params.minVal - 1);		

        ParseString(values, [&](char Char, const std::vector<char> &SpecCharsLog, int)
        {
            if(Char == ',' && SpecCharsLog.size() == 0)
                isSecondVal = true;
            else
                (isSecondVal ? secondVal : firstVal) += Char;
        });

		if(firstVal == "" || secondVal == "") throw FormatException("invalid string " + String);
		
		return std::make_pair(Parser::FromString(firstVal), Parser::FromString(secondVal));
	}
	static std::string PairToString(const Pair &DataPair, char OpenSymbol, char CloseSymbol) throw (FormatException)
	{
		std::ostringstream sstrm;
		sstrm << OpenSymbol << Parser::ToString(DataPair.first) << ',' << Parser::ToString(DataPair.second) << CloseSymbol;
		return sstrm.str();
	}
	static Pair PairFromBinary(const char* Data)
	{
		Pair outPair;
		outPair.first = Parser::FromBinary(Data);
		outPair.second = Parser::FromBinary(Data + sizeof(Parser::ValueType));	
		return outPair;
	}
	static void PairToBinary(const Pair &DataPair, char* Data)
	{		
		Parser::ToBinary(DataPair.first, Data);
		Parser::ToBinary(DataPair.second, Data + sizeof(Parser::ValueType));
 	}
	typedef std::vector<typename Parser::ValueType> Array;
	static Array ArrayFromString(const std::string &String, char OpenSymbol, char CloseSymbol, char Delimiter) throw (FormatException)
	{
		RangeI params = GetStatementParameters(String, OpenSymbol, CloseSymbol);
		
		if(params.minVal == -1 || params.maxVal == -1) throw FormatException("invalid string " + String);
			
		std::string tmpVar = "";
		Array outData;
		std::string values = String.substr(params.minVal + 1, params.maxVal - params.minVal - 1);	

        ParseString(values, [&](char Char, const std::vector<char> &SpecCharsLog, int)
        {
            if(Char == Delimiter && SpecCharsLog.size() == 0 && tmpVar != ""){
                outData.push_back(Parser::FromString(tmpVar));
                tmpVar = "";
            }else
                tmpVar += Char;
        });

		if(tmpVar != "")outData.push_back(Parser::FromString(tmpVar));

		return outData;
	}
	static std::string ArrayToString(const Array &Data, char OpenSymbol, char CloseSymbol, char Delimiter) throw (FormatException)
	{
		std::ostringstream sstrm;		
		sstrm << OpenSymbol;
		for(size_t i = 0, size = Data.size(); i < size; i++){
			sstrm << Parser::ToString(Data[i]);
			if(i < size - 1)sstrm << Delimiter;
		}
		sstrm << CloseSymbol;
		return sstrm.str();
	}	
	static Array ArrayFromBinary(const char* Data)
	{
		Array outArray;
		const size_t arrSize = *reinterpret_cast<const size_t*>(Data);
		Data += sizeof(size_t);
		for(size_t i = 0; i < arrSize; i++){
			outArray.push_back(Parser::FromBinary(Data));
			Data += sizeof(typename Parser::ValueType);
		}
		return outArray;
	}
	static void ArrayToBinary(const Array &DataArr, char* Data)
	{	
		*reinterpret_cast<size_t*>(Data) = DataArr.size();
		Data += sizeof(size_t);
		for(size_t i = 0, size = DataArr.size(); i < size; i++){
			Parser::ToBinary(DataArr[i], Data);
			Data += sizeof(typename Parser::ValueType);
		}
 	}
};

template<typename T>
class NumberConverter
{
public:
	static T FromString(const std::string &String) throw (Exception)
	{ 			
		RangeI params = GetStatementParameters(String, '<', '>');

		if(params.minVal == -1 && params.maxVal == -1){
			std::string lowerCase = String;
			std::transform(lowerCase.begin(), lowerCase.end(), lowerCase.begin(), ::tolower);
			if(lowerCase.find("0x") != std::string::npos){
				long int outVar;			    
				std::stringstream ss;
				ss << std::hex << lowerCase;
				ss >> outVar;
				return static_cast<T>(outVar);
			}

			return ParseNumber<T>(String);
		}

        return GetRandomFromRange<int>(String, 
            [](int MinVal, int MaxVal)
            {
                return MinVal + (rand() % ((MaxVal - MinVal) + 1));
            }
        );
	}
};

template<>
class NumberConverter<float>
{
public:
	static float FromString(const std::string &String)
	{
		RangeI params = GetStatementParameters(String, '<', '>');

		if(params.minVal == -1 && params.maxVal == -1)
			return ParseNumber<float>(String);
		
        return GetRandomFromRange<float>(String,
            [](float MinVal, float MaxVal)
            {
                return MinVal + ((MaxVal - MinVal) * (static_cast<float>(rand() % 100) * 0.01f));
            }
        );
	}
};

template<>
class NumberConverter<bool>
{
private:
	static bool ParseVal(const std::string &String)
	{
		std::string str = String;
		std::transform(str.begin(), str.end(), str.begin(), ::tolower);

		bool outVal = false;
		if(str == "true" || str == "yes" || str == "1")
			outVal = true;
		else if(str == "false" || str == "no" || str == "0")
			outVal = false;
		else throw FormatException("Invalid value " + String);

		return outVal;
	}
public:
	static bool FromString(const std::string &String)
	{
		RangeI params = GetStatementParameters(String, '<', '>');

		if(params.minVal == -1 && params.maxVal == -1){			
			return ParseVal(String);			
		}else if(params.minVal == -1 || params.maxVal == -1)
			throw FormatException("invalid random value syntax for sring " + String);

		std::string minValStr, maxValStr;
		
		bool isSecondVal = false;
		std::string values = String.substr(params.minVal + 1, params.maxVal - params.minVal - 1);
		
		values.erase(remove_if(values.begin(), values.end(), isspace), values.end());

		for(size_t i = 0; i < values.size(); i++){
			if(values[i] == ',') {isSecondVal = true; continue;}
			(isSecondVal ? maxValStr : minValStr) += values[i];	
		}	
		
		ParseVal(minValStr);
		ParseVal(maxValStr);

		return rand() % 2 == 1;
	}
};

template<typename T, class Converter = NumberConverter<T> >
class NumericParser
{
public:
	typedef T ValueType;
	static T FromString(const std::string &String) throw (FormatException)
	{
        return ParseValue<NumericParser<T, Converter>>(String, Converter::FromString);
	}
	static std::string ToString(const T &Value)
	{
		std::ostringstream sstrm;
		sstrm << Value;
		return sstrm.str();
	}
	static T FromBinary(const char* Data)
	{
		return *((T*)Data);
	}
	static void ToBinary(const T &Value, char *OutData)
	{				
		memcpy(OutData, (char*)&Value, sizeof(T));
	}
};

template<typename Parser, typename VectorType = BasePoint2<typename Parser::ValueType> >
class BasePoint2Parser
{
public:
	typedef VectorType ValueType;
	static VectorType FromString(const std::string &String) throw (FormatException)
	{
        typedef BasePoint2Parser<Parser, VectorType> ThisType;
        return ParseValue<ThisType>(String, 
            [](const std::string &Str)
            {
                SerializerTools<Parser>::Pair pair = SerializerTools<Parser>::PairFromString(Str,'(', ')');
                return VectorType(pair.first, pair.second);
            }
        );
	}
	static std::string ToString(const VectorType &Data) throw (FormatException)
	{
		typename SerializerTools<Parser>::Pair pair(Data.x, Data.y);
		return SerializerTools<Parser>::PairToString(pair, '(', ')');
	}
	static VectorType FromBinary(const char *RawData)
	{
		typename SerializerTools<Parser>::Pair pair = SerializerTools<Parser>::PairFromBinary(RawData);
		return VectorType(pair.first, pair.second);
	}
	static void ToBinary(const VectorType &Data, char *RawData)
	{
		typename SerializerTools<Parser>::Pair pair(Data.x, Data.y);
		SerializerTools<Parser>::PairToBinary(pair, RawData);
	}
};

template<typename Parser, typename PointType = BasePoint3<typename Parser::ValueType> >
class BasePoint3Parser
{
public:
    typedef PointType ValueType;
    static PointType FromString(const std::string &String)
    {
        typedef BasePoint3Parser<Parser, PointType> ThisType;
        return ParseValue<ThisType>(String, 
            [](const std::string &Str) -> PointType
            {
                SerializerTools<Parser>::Array arr = SerializerTools<Parser>::ArrayFromString(Str, '(', ')', ',');
                
                if(arr.size() != 3)
                    throw FormatException("cant cast " + Str + " to BasePoint3");

                return {arr[0], arr[1], arr[2]};
            }
        );
    }
    static std::string ToString(const PointType &Data)
    {
        return SerializerTools<Parser>::ArrayToString({Data.x, Data.y, Data.z}, '(', ')', ',');
    }
    static PointType FromBinary(const char *RawData)
    {
        PointType data;
        data.x = Parser::FromBinary(RawData); RawData += sizeof(Parser::ValueType);
        data.y = Parser::FromBinary(RawData); RawData += sizeof(Parser::ValueType);
        data.z = Parser::FromBinary(RawData); RawData += sizeof(Parser::ValueType);

        return data;
    }
    static void ToBinary(const PointType &Data, char *RawData)
    {
        Parser::ToBinary(Data.x, RawData); RawData += sizeof(Parser::ValueType);
        Parser::ToBinary(Data.y, RawData); RawData += sizeof(Parser::ValueType);
        Parser::ToBinary(Data.z, RawData); RawData += sizeof(Parser::ValueType);
    }
};

template<typename Parser, typename SizeType = Size<typename Parser::ValueType> >
class SizeParser
{
public:
	typedef SizeType ValueType;
	static SizeType FromString(const std::string &String)
	{
        typedef SizeParser<Parser, SizeType> ThisType;
        return ParseValue<ThisType>(String, 
            [](const std::string &Str)
            {
                SerializerTools<Parser>::Pair pair = SerializerTools<Parser>::PairFromString(Str, '(', ')');
                return SizeType(pair.first, pair.second);
            }
        );
	}
	static std::string ToString(const SizeType &Data)
	{		 
		return SerializerTools<Parser>::PairToString(SerializerTools<Parser>::Pair(Data.width, Data.height), '(', ')');
	}
	static SizeType FromBinary(const char *RawData)
	{
		typename SerializerTools<Parser>::Pair pair = SerializerTools<Parser>::PairFromBinary(RawData);
		return SizeType(pair.first, pair.second);
	}
	static void ToBinary(const SizeType &Data, char *RawData)
	{		 
		SerializerTools<Parser>::PairToBinary(SerializerTools<Parser>::Pair(Data.width, Data.height), RawData);
	}
};

template<typename Parser, typename RangeType = Range<typename Parser::ValueType> >
class RangeParser
{
public:
	typedef RangeType ValueType;
	static RangeType FromString(const std::string &String) throw (FormatException)
	{
        typedef RangeParser<Parser, RangeType> ThisType;
        return ParseValue<ThisType>(String, 
            [](const std::string &Str)
            {
                SerializerTools<Parser>::Pair pair = SerializerTools<Parser>::PairFromString(Str, '(', ')');
                if(pair.first > pair.second)
                    throw FormatException("min val is greater than max val for range " + Str);
                return RangeType(pair.first, pair.second);
            }
        );
	}
	static std::string ToString(const RangeType &Range)
	{
        return SerializerTools<Parser>::PairToString(SerializerTools<Parser>::Pair(Range.minVal, Range.maxVal), '(', ')');
	}
	static RangeType FromBinary(const char *Data)
	{
		typename SerializerTools<Parser>::Pair pair = SerializerTools<Parser>::PairFromBinary(Data);
		return RangeType(pair.first, pair.second);
	}
	static void ToBinary(const RangeType &Range, char *Data) 
	{
		SerializerTools<Parser>::PairToBinary(SerializerTools<Parser>::Pair(Range.minVal, Range.maxVal), Data);
	}
};

template<typename Parser>
class ArrayParser
{
public:
	typedef typename SerializerTools<Parser>::Array ValueType;	
	static ValueType FromString(const std::string &String)
	{
        typedef ArrayParser<Parser> ThisType;
        return ParseValue<ThisType>(String, 
            [](const std::string &Str)
            {
                return SerializerTools<Parser>::ArrayFromString(Str, '(', ')', ',');
            }
        );
	}
	static std::string ToString(const ValueType &Array)
	{
		return SerializerTools<Parser>::ArrayToString(Array, '(', ')', ',');
	}
	static ValueType FromBinary(const char *Data)
	{
		return SerializerTools<Parser>::ArrayFromBinary(Data);
	}
	static void ToBinary(const ValueType &Array, char *Data) 
	{
		return SerializerTools<Parser>::ArrayToBinary(Array, Data);
	}
};

class StringParser
{
public:
	typedef std::string ValueType;
	static std::string FromString(const std::string &String) throw (FormatException) 
	{
        return ParseValue<StringParser>(String, [](const std::string &Str) -> std::string {return Str;});
	}
	static std::string ToString(const std::string &String) 
	{
		return String;
	}
	static std::string FromBinary(const char* Data)
	{
		std::string outString;
		size_t size = *((size_t*)Data); Data += sizeof(size_t);
		for(size_t i = 0; i < size; i++)
			outString += Data[i];
		return outString;
	}
	static void ToBinary(const std::string &Value, char *OutData)
	{
		*((size_t*)OutData) = Value.length(); OutData += sizeof(size_t);
		for(size_t i = 0; i <  Value.length(); i++)
			OutData[i] = Value[i];
	}
};

template<typename Parser, typename ColorType = BaseColor<typename Parser::ValueType> >
class ColorParser
{
public:
    typedef ColorType ValueType;
	static ValueType FromString(const std::string &String)
	{
        return ParseValue<ColorParser<Parser, ValueType>>(String, 
            [](const std::string &Str)
            {
                ArrayParser<Parser>::ValueType arr = ArrayParser<Parser>::FromString(Str);
                if(arr.size() != 4) 
                    throw FormatException("invalid color syntax for string " + Str);

                ValueType outData;
                outData.b = arr[0];
                outData.g = arr[1];
                outData.r = arr[2];
                outData.a = arr[3];
                return outData;
            }
        );
	}
	static std::string ToString(const ValueType &Color)
	{
		typename SerializerTools<Parser>::Array arr;
		arr.push_back(Color.b);
		arr.push_back(Color.g);
		arr.push_back(Color.r);
		arr.push_back(Color.a);
		return SerializerTools<Parser>::ArrayToString(arr, '(', ')', ',');
	}
	static ValueType FromBinary(const char *Data)
	{
		Channels outData;
		outData.b = Parser::FromBinary(Data); Data += sizeof(Parser::ValueType);
		outData.g = Parser::FromBinary(Data); Data += sizeof(Parser::ValueType);
		outData.r = Parser::FromBinary(Data); Data += sizeof(Parser::ValueType);
		outData.a = Parser::FromBinary(Data); Data += sizeof(Parser::ValueType);
		return outData;
	}
	static void ToBinary(const ValueType &Color, char *Data) 
	{		
		Parser::ToBinary(Color.b, Data); Data += sizeof(Parser::ValueType);
		Parser::ToBinary(Color.g, Data); Data += sizeof(Parser::ValueType);
		Parser::ToBinary(Color.r, Data); Data += sizeof(Parser::ValueType);
		Parser::ToBinary(Color.a, Data); Data += sizeof(Parser::ValueType);
	}
};

typedef NumericParser<int> IntParser;
typedef NumericParser<unsigned short> USParser;
typedef NumericParser<unsigned char> UCharParser;
typedef NumericParser<float> FloatParser;

typedef RangeParser<USParser> RangeUSParser;
typedef RangeParser<FloatParser> RangeFParser;
typedef Range<IntParser> RangeIParser;

typedef BasePoint2Parser<IntParser> Point2Parser;
typedef BasePoint2Parser<FloatParser> Point2FParser;
typedef BasePoint2Parser<FloatParser, Vector2> Vector2Parser;

typedef BasePoint3Parser<IntParser> Point3Parser;
typedef BasePoint3Parser<FloatParser> Point3FParser;
typedef BasePoint3Parser<FloatParser, Vector3> Vector3Parser;

template<class TParser> 
using BaseVector3Parser = BasePoint3Parser<TParser, BaseVector3<typename TParser::ValueType>>;

typedef SizeParser<USParser> SizeUSParser;
typedef SizeParser<FloatParser> SizeFParser;
typedef SizeParser<IntParser> SizeIParser;

typedef ColorParser<UCharParser> ColorUCharParser;
typedef ColorParser<FloatParser> ColorFParser;

template<typename T>
class BinarySerializerSize
{
public:
    static size_t Get(T Var){ return sizeof(T);}
};

template<>
class BinarySerializerSize<std::string>
{
public:
    static size_t Get(const std::string &Var){ return Var.length() + sizeof(size_t);}    
};

template<typename T>
class BinarySerializerSize<std::vector<T> >
{
public:
    static size_t Get(const std::vector<T> &Var){ return Var.size() + (sizeof(T) * Var.size());}
};

template<typename T>
struct ParserType
{
    typedef NumericParser<T> Type;
};

template<typename T>
struct ParserType<std::vector<T> >
{
    typedef ArrayParser<T> Type;
};

template<>
struct ParserType<std::string>
{
    typedef StringParser Type;
};