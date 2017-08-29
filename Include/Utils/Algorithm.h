/*******************************************************************************
    Author: Alexey Frolov (alexwin32@mail.ru)

    This software is distributed freely under the terms of the MIT License.
    See "LICENSE" or "http://copyfree.org/content/standard/licenses/mit/license.txt".
*******************************************************************************/

#pragma once 
#include <vector>
#include <map>
#include <stdint.h>

namespace Utils
{

template<class TVar>
inline void Remove(std::vector<TVar> &Data, const TVar &Value)
{
    typename std::vector<TVar>::iterator it;
    for(it = Data.begin(); it != Data.end(); ++it)
        if(*it == Value){
            Data.erase(it);
            return;
        }
}

template<class TVar, class Func>
inline void Remove(std::vector<TVar> &Data, const TVar &Value, Func Function)
{
    typename std::vector<TVar>::iterator it;
    for(it = Data.begin(); it != Data.end(); ++it)
        if(*it == Value){
            Function(*it);
            Data.erase(it);
            return;
        }
}

template<class TVar>
inline int32_t FindIndex(const std::vector<TVar> &Data, const TVar &Value)
{
    for(size_t i = 0; i < Data.size(); i++)
        if(Data[i] == Value)
            return (int32_t)i;

    return -1;
}

template<class TVar>
inline std::vector<TVar> Concatenate(const std::vector<TVar> &A, const std::vector<TVar> &B)
{
    std::vector<TVar> out;

    out.reserve(A.size() + B.size());

    out.insert(out.end(), A.begin(), A.end());
    out.insert(out.end(), B.begin(), B.end());

    return out;
}

template<class TContainer, class TFunc>
inline auto FindMaxValue(const TContainer &Data,
                         const TFunc &Pred)-> decltype(Pred(*Data.begin()))
{
    using TVal = decltype(Pred(*Data.begin()));

    if(Data.size() == 0)
        return TVal();

    TVal maxVal = Pred(*Data.begin());

    for(const auto &var : Data){

        TVal val = Pred(var);
        
        if(maxVal < val)
            maxVal = val;
    }

    return maxVal;
}

template<class TContainer, class TFunc>
auto FindMinValue(const TContainer &Data,
                  const TFunc &Pred) -> decltype(Pred(*Data.begin()))
{
    using TVal = decltype(Pred(*Data.begin()));

    if(Data.size() == 0)
        return TVal();

    TVal minVal = Pred(*Data.begin());

    for(const auto &var : Data){

        TVal val = Pred(var);
        
        if(minVal > val)
            minVal = val;
    }

    return minVal;
}

template<class TKey, class TValue, class TException>
inline const TValue &Find(const std::map<TKey, TValue> &Data,
                          const TKey &Key,
                          const TException &NotFoundException)
{
    auto it = Data.find(Key);

    if(it == Data.end())
        throw NotFoundException;

    return it->second;
}


template<class TKey, class TValue, class TException>
inline TValue &Find(std::map<TKey, TValue> &Data,
                    const TKey &Key,
                    const TException &NotFoundException)
{
    auto it = Data.find(Key);

    if(it == Data.end())
        throw NotFoundException;

    return it->second;
}

template<class TVal>
inline void AddToStream(char* &Ptr, const TVal &Value)
{
    *reinterpret_cast<TVal*>(Ptr) = Value;
    Ptr += sizeof(TVal);
}

template<class TString, class TDelimiter>
inline std::vector<TString> SplitString(const TString &String, TDelimiter Delimiter, bool AlowEmptyElements = true)
{
    std::vector<TString> splStr;
    TString token;

    for(auto ch : String)
        if (ch == Delimiter){

            if((!AlowEmptyElements && !token.empty()) || AlowEmptyElements)
                splStr.push_back(token);

            token.clear();
        }else
            token += ch;

    if((!AlowEmptyElements && !token.empty()) || AlowEmptyElements)
        splStr.push_back(token);

    return splStr;
}

template<class TString>
inline std::vector<TString> SplitString(const TString &String, const TString &DelimiterString, bool AlowEmptyElements = true)
{
    std::vector<TString> splStr;
    TString token;

    size_t matchInd = 0;

    for(auto ch : String)
        if(ch == DelimiterString[matchInd]){
            matchInd++;

            if(matchInd == DelimiterString.size()){

                if((!AlowEmptyElements && !token.empty()) || AlowEmptyElements)
                    splStr.push_back(token);

                token.clear();
                matchInd = 0;
            }

        }else{
            if(matchInd != 0){
                token += DelimiterString.substr(0, matchInd);
                matchInd = 0;
            }else
                token += ch;
        }

    if((!AlowEmptyElements && !token.empty()) || AlowEmptyElements)
        splStr.push_back(token);

    return splStr;
}

}