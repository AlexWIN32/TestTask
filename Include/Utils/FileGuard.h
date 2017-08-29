/*******************************************************************************
    Author: Alexey Frolov (alexwin32@mail.ru)

    This software is distributed freely under the terms of the MIT License.
    See "LICENSE" or "http://copyfree.org/content/standard/licenses/mit/license.txt".
*******************************************************************************/

#pragma once
#include <cstdio>
#include <string>
#include <stdint.h>
#include <Exception.h>

namespace Utils
{

class DefaultSerialisingStrategy
{
public:
    template<class TVar>
    static void Write(FILE *File, const std::string &Path, const TVar &Var)
    {
        if(fwrite(&Var, sizeof(Var), 1, File) != 1)
            throw IOException("cant write to file " + Path);
    }
    template<>
    static void Write<std::string>(FILE *File, const std::string &Path, const std::string &Var)
    {
        for(char ch : Var)
            Write(File, Path, ch);

        Write<char>(File, Path, '\0');
    }
    template<class TVar>
    static TVar Read(FILE *File, const std::string &Path, bool &Eof)
    {
        TVar var;
        if(fread(&var, sizeof(TVar), 1, File) != 1 && !feof(File))
            throw IOException("cant read from file " + Path);

        Eof = feof(File) != 0;

        return var;
    }
    template<>
    static std::string Read<std::string>(FILE *File, const std::string &Path, bool &Eof)
    {
        return ReadUntil<std::string, uint8_t>(File, Path, Eof, '\0');
    }
    template<class TContainer, class TVar>
    static TContainer ReadUntil(FILE *File,
                                const std::string &Path,
                                bool &Eof,
                                const TVar &TerminateElement)
    {
        TContainer container;

        while(true){

            TVar var = Read<TVar>(File, Path, Eof);

            if(Eof)
                break;

            if(var == TerminateElement)
                break;
            else
                container.push_back(var);
        }

        return container;
    }
};

template<class TStrategy>
class BasicFileGuard final
{
private:
    FILE *file = nullptr;
    bool eof = false;
    std::string path;
public:
    BasicFileGuard(const BasicFileGuard &) = delete;
    BasicFileGuard &operator= (const BasicFileGuard &) = delete;
    BasicFileGuard(FILE *File, const std::string &Path) : file(File), path(Path){}
    BasicFileGuard(const std::string &Path, const std::string &Mode) throw (Exception)
    {
        if(fopen_s(&file, Path.c_str(), Mode.c_str()))
            throw IOException("Cant open file " + Path);

        path = Path;
    }
    FILE *get() const {return file;}
    ~BasicFileGuard() { if(file) fclose(file);}
    operator const FILE *() const {return file;}
    operator FILE *() {return file;}
    template<class TVar>
    TVar Read() throw (Exception)
    {
        return TStrategy::Read<TVar>(file, path, eof);
    }
    template<class TContainer, class TVar>
    TContainer ReadUntil(const TVar &TerminateElement)
    {
        return TStrategy::ReadUntil<TContainer>(file, path, eof, TerminateElement);
    }
    template<class TVar>
    void Write(const TVar &Var)
    {
        TStrategy::Write<TVar>(file, path, Var);
    }
    bool Eof() const {return eof;}
};

typedef BasicFileGuard<DefaultSerialisingStrategy> FileGuard;
}