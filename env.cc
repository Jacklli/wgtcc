#include "env.h"
#include "ast.h"
#include "error.h"

using namespace std;


/************** Env ******************/

Variable* Env::Find(const std::string& name)
{
    auto symb = _symbMap.find(name);
    if (_symbMap.end() != symb)
        return symb->second;
    if (nullptr == _parent)
        return nullptr;

    return _parent->Find(name);
}

const Variable* Env::Find(const std::string& name) const
{
    auto thi = const_cast<Env*>(this);
    return const_cast<const Variable*>(thi->Find(name));
}

Type* Env::FindTypeInCurScope(const std::string& name)
{
    auto type = _symbMap.find(name);
    if (_symbMap.end() == type)
        return nullptr;

    return type->second->IsVar() ? nullptr : type->second->Ty();
}

const Type* Env::FindTypeInCurScope(const std::string& name) const
{
    auto thi = const_cast<Env*>(this);
    return const_cast<const Type*>(thi->FindTypeInCurScope(name));
}

Variable* Env::FindVarInCurScope(const std::string& name)
{
    auto var = _symbMap.find(name);
    if (_symbMap.end() == var)
        return nullptr;

    return var->second->IsVar() ? var->second : nullptr;
}

const Variable* Env::FindVarInCurScope(const std::string& name) const
{
    auto thi = const_cast<Env*>(this);
    return const_cast<const Variable*>(thi->FindVarInCurScope(name));
}

Type* Env::FindType(const std::string& name)
{
    auto symb = Find(name);
    if (nullptr == symb)
        return nullptr;
    
    //found the type in current scope
    return symb->IsVar() ? nullptr : symb->Ty();
} 

const Type* Env::FindType(const std::string& name) const
{
    auto thi = const_cast<Env*>(this);
    return const_cast<const Type*>(thi->FindType(name));
}

Variable* Env::FindVar(const std::string& name)
{
    auto symb = Find(name);
    if (nullptr == symb)
        return nullptr;
    
    //found the type in current scope
    return symb->IsVar() ? symb : nullptr;
}

const Variable* Env::FindVar(const std::string& name) const
{
    auto thi = const_cast<Env*>(this);
    return const_cast<const Variable*>(thi->FindVar(name));
}

/*
�������ֲ��ڷ��ű��ڣ���ô�ڵ�ǰ�������������֣�
���������Ѿ��ڷ��ű��ڣ���ô���������ͣ�������Ȼ�������ڴ�й¶���ɵĲ��������Ϳ��ܱ�����Ҳ����û�б����ã�
������ͻӦ���ɵ��÷����飻
*/
void Env::InsertType(const std::string& name, Variable* var)
{
    assert(!var->IsVar());
    //auto iter = _symbMap.find(name);
    //�������������������ͣ�Ҳ���������ǲ����������ͣ���Ӧ���޸Ĵ˲�����������Ϊ������
    assert(nullptr == FindTypeInCurScope(name));
    //assert(!iter->second->Ty()->IsComplete());
    //auto var = TranslationUnit::NewVariable(type, Variable::TYPE);
    _symbMap[name] = var;
}

void Env::InsertVar(const std::string& name, Variable* var)
{
    assert(var->IsVar());
    //�������ظ����壬��Ӧ���ɵ��÷�����
    assert(nullptr == FindVarInCurScope(name));
    //auto var = TranslationUnit::NewVariable(type, _offset);
    var->SetOffset(_offset);
    
    _symbMap[name] = var;
    _offset += var->Ty()->Align();
}

bool Env::operator==(const Env& other) const
{
    if (this->_symbMap.size() != other._symbMap.size())
        return false;

    auto iterThis = this->_symbMap.begin();
    auto iterOther = other._symbMap.begin();
    for (; iterThis != this->_symbMap.end(); iterThis++, iterOther++) {
        if (iterThis->first != iterOther->first)
            return false;
        if (*iterThis->second != *iterOther->second)
            return false;
    }

    return true;
}

Constant* Env::InsertConstant(const std::string& name, Constant* constant) {
    //ע��enumeration constant ��һ����������ͬһ�����ռ�
    assert(nullptr == FindVarInCurScope(name));
    _symbMap[name] = constant;
    
    return constant;
}

Type* Env::FindTagInCurScope(const std::string& tag)
{
    auto type = _tagMap.find(tag);
    if (_tagMap.end() == type)
        return nullptr;

    return type->second;
}

Type* Env::InsertTag(const std::string& tag, Type* type)
{
    assert(nullptr == FindTagInCurScope(tag));
    _tagMap[tag] = type;

    return type;
}

Type* Env::FindTag(const std::string& tag)
{
    auto type = FindTagInCurScope(tag);
    if (nullptr != type)
        return type;
    if (nullptr == _parent)
        return nullptr;

    return _parent->FindTag(tag);
}
