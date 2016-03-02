/*
 * Copyright 2015 Intel(r) Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http ://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */
#include "vmf/statistics.hpp"
#include "vmf/metadata.hpp"
#include "vmf/metadatastream.hpp"
//#include <cstring>
//#include <string>
//#include <memory>
//#include <sstream>
//#include <cmath>
//#include <limits>
//#include <iomanip>

namespace vmf
{

// struct StatUpdateMode
#define CASE_ITEM(x) case x:  return #x
std::string StatUpdateMode::toString( StatUpdateMode::Type val )
{
    switch( val )
    {
        CASE_ITEM(StatUpdateMode::Disabled);
        CASE_ITEM(StatUpdateMode::Manual);
        CASE_ITEM(StatUpdateMode::OnAdd);
        CASE_ITEM(StatUpdateMode::OnTimer);
    }
    VMF_EXCEPTION( vmf::IncorrectParamException, "Unknown enum value: " + std::to_string((int) val) );
}
#undef CASE_ITEM

#define STR2VAL(x)  if( str == #x ) return x
StatUpdateMode::Type StatUpdateMode::fromString( const std::string& str )
{
    STR2VAL(StatUpdateMode::Disabled);
    STR2VAL(StatUpdateMode::Manual);
    STR2VAL(StatUpdateMode::OnAdd);
    STR2VAL(StatUpdateMode::OnTimer);
    VMF_EXCEPTION(vmf::IncorrectParamException, "Unknown enum string: " + str);
}
#undef STR2VAL

// class StatOpBase: builtin operations

class StatOpMin: public StatOpBase
{
public:
    StatOpMin()
        {}
    virtual ~StatOpMin()
        {}

public:
    virtual std::string name() const
        { return StatOpFactory::builtinName(StatOpFactory::BuiltinOp::Min); }
    virtual void reset()
        { m_value = Variant(); }
    virtual bool handle( StatAction::Type action, const Variant& inputValue )
        {
            switch( action )
            {
            case StatAction::Add:
                if( m_value.isEmpty() )
                    m_value = inputValue;
                else if( m_value.getType() != inputValue.getType() )
                    VMF_EXCEPTION( vmf::TypeCastException, "Type mismatch" );
                else
                    switch( m_value.getType() )
                    {
                    case Variant::type_integer:
                        if( inputValue.get_integer() < m_value.get_integer() )
                            m_value = inputValue;
                        break;
                    case Variant::type_real:
                        if( inputValue.get_real() < m_value.get_real() )
                            m_value = inputValue;
                        break;
                    default:
                        VMF_EXCEPTION( vmf::NotImplementedException, "Operation not applicable to this data type" );
                    }
                break;
            case StatAction::Remove:
                if( m_value.isEmpty() )
                    VMF_EXCEPTION( vmf::NotImplementedException, "Operation not applicable to this data type" );
                else if( m_value.getType() != inputValue.getType() )
                    VMF_EXCEPTION( vmf::TypeCastException, "Type mismatch" );
                switch( m_value.getType() )
                {
                case Variant::type_integer:
                    return false;
                case Variant::type_real:
                    return false;
                default:
                    VMF_EXCEPTION( vmf::NotImplementedException, "Operation not applicable to this data type" );
                }
            break;
            }
            return true;
        }
    virtual const Variant& value() const
        { return m_value; }

private:
    Variant m_value;

public:
    static StatOpBase* createInstance()
        { return new StatOpMin(); }
};

class StatOpMax: public StatOpBase
{
public:
    StatOpMax()
        {}
    virtual ~StatOpMax()
        {}

public:
    virtual std::string name() const
        { return StatOpFactory::builtinName(StatOpFactory::BuiltinOp::Max); }
    virtual void reset()
        { m_value = Variant(); }
    virtual bool handle( StatAction::Type action, const Variant& inputValue )
        {
            switch( action )
            {
            case StatAction::Add:
                if( m_value.isEmpty() )
                    m_value = inputValue;
                else if( m_value.getType() != inputValue.getType() )
                    VMF_EXCEPTION( vmf::TypeCastException, "Type mismatch" );
                else
                    switch( m_value.getType() )
                    {
                    case Variant::type_integer:
                        if( inputValue.get_integer() > m_value.get_integer() )
                            m_value = inputValue;
                        break;
                    case Variant::type_real:
                        if( inputValue.get_real() > m_value.get_real() )
                            m_value = inputValue;
                        break;
                    default:
                        VMF_EXCEPTION( vmf::NotImplementedException, "Operation not applicable to this data type" );
                    }
                break;
            case StatAction::Remove:
                if( m_value.isEmpty() )
                    VMF_EXCEPTION( vmf::NotImplementedException, "Operation not applicable to this data type" );
                else if( m_value.getType() != inputValue.getType() )
                    VMF_EXCEPTION( vmf::TypeCastException, "Type mismatch" );
                switch( m_value.getType() )
                {
                case Variant::type_integer:
                    return false;
                case Variant::type_real:
                    return false;
                default:
                    VMF_EXCEPTION( vmf::NotImplementedException, "Operation not applicable to this data type" );
                }
                break;
            }
            return true;
        }
    virtual const Variant& value() const
        { return m_value; }

private:
    Variant m_value;

public:
    static StatOpBase* createInstance()
        { return new StatOpMax(); }
};

class StatOpAverage: public StatOpBase
{
public:
    StatOpAverage()
        : m_count( 0 ) {}
    virtual ~StatOpAverage()
        {}

public:
    virtual std::string name() const
        { return StatOpFactory::builtinName(StatOpFactory::BuiltinOp::Average); }
    virtual void reset()
        { m_count = 0; m_value = Variant(); }
    virtual bool handle( StatAction::Type action, const Variant& inputValue )
        {
            switch( action )
            {
            case StatAction::Add:
                if( m_value.isEmpty() )
                    { m_count = 1; m_value = inputValue; }
                else if( m_value.getType() != inputValue.getType() )
                    VMF_EXCEPTION( vmf::TypeCastException, "Type mismatch" );
                else
                    switch( m_value.getType() )
                    {
                    case Variant::type_integer:
                        ++m_count; m_value = Variant( m_value.get_integer() + inputValue.get_integer() );
                        break;
                    case Variant::type_real:
                        ++m_count; m_value = Variant( m_value.get_real() + inputValue.get_real() );
                        break;
                    default:
                        VMF_EXCEPTION( vmf::NotImplementedException, "Operation not applicable to this data type" );
                    }
                break;
            case StatAction::Remove:
                if( m_value.isEmpty() )
                    VMF_EXCEPTION( vmf::NotImplementedException, "Operation not applicable to this data type" );
                else if( m_value.getType() != inputValue.getType() )
                    VMF_EXCEPTION( vmf::TypeCastException, "Type mismatch" );
                switch( m_value.getType() )
                {
                case Variant::type_integer:
                    --m_count; m_value = Variant( m_value.get_integer() - inputValue.get_integer() );
                    break;
                case Variant::type_real:
                    --m_count; m_value = Variant( m_value.get_real() - inputValue.get_real() );
                    break;
                default:
                    VMF_EXCEPTION( vmf::NotImplementedException, "Operation not applicable to this data type" );
                }
                break;
            }
            return true;
        }
    virtual const Variant& value() const
        {
            switch( m_value.getType() )
            {
            case Variant::type_unknown: // isEmpty()
                break;
            case Variant::type_integer:
                m_temp = Variant( ((vmf_real) m_value.get_integer()) / m_count );
                return m_temp;
            case Variant::type_real:
                m_temp = Variant( ((vmf_real) m_value.get_real()) / m_count );
                return m_temp;
            default:
                VMF_EXCEPTION( vmf::NotImplementedException, "Operation not applicable to this data type" );
            }
            return m_value;
        }

private:
    Variant m_value;
    vmf_integer m_count;
    mutable Variant m_temp; // temp return value for getValue()

public:
    static StatOpBase* createInstance()
        { return new StatOpAverage(); }
};

class StatOpCount: public StatOpBase
{
public:
    StatOpCount()
        : m_count( 0 ) {}
    virtual ~StatOpCount()
        {}

public:
    virtual std::string name() const
        { return StatOpFactory::builtinName(StatOpFactory::BuiltinOp::Count); }
    virtual void reset()
        { m_count = 0; }
    virtual bool handle( StatAction::Type action, const Variant& /*inputValue*/ )
        {
            switch( action )
            {
            case StatAction::Add:
                ++m_count;
                break;
            case StatAction::Remove:
                if( !(m_count > 0) )
                    VMF_EXCEPTION( vmf::NotImplementedException, "Operation not applicable to this data type" );
                --m_count;
                break;
            }
            return true;
        }
    virtual const Variant& value() const
        { m_temp = Variant( (vmf_integer)m_count ); return m_temp; }

private:
    vmf_integer m_count;
    mutable Variant m_temp; // temp return value for getValue()

public:
    static StatOpBase* createInstance()
        { return new StatOpCount(); }
};

class StatOpSum: public StatOpBase
{
public:
    StatOpSum()
        {}
    virtual ~StatOpSum()
        {}

public:
    virtual std::string name() const
        { return StatOpFactory::builtinName(StatOpFactory::BuiltinOp::Sum); }
    virtual void reset()
        { m_value = Variant(); }
    virtual bool handle( StatAction::Type action, const Variant& inputValue )
        {
            switch( action )
            {
            case StatAction::Add:
                if( m_value.isEmpty() )
                    { m_value = inputValue; }
                else if( m_value.getType() != inputValue.getType() )
                    VMF_EXCEPTION( vmf::TypeCastException, "Type mismatch" );
                else
                    switch( m_value.getType() )
                    {
                    case Variant::type_integer:
                        m_value = Variant( m_value.get_integer() + inputValue.get_integer() );
                        break;
                    case Variant::type_real:
                        m_value = Variant( m_value.get_real() + inputValue.get_real() );
                        break;
                    default:
                        VMF_EXCEPTION( vmf::NotImplementedException, "Operation not applicable to this data type" );
                    }
                break;
            case StatAction::Remove:
                if( m_value.isEmpty() )
                    VMF_EXCEPTION( vmf::NotImplementedException, "Operation not applicable to this data type" );
                else if( m_value.getType() != inputValue.getType() )
                    VMF_EXCEPTION( vmf::TypeCastException, "Type mismatch" );
                switch( m_value.getType() )
                {
                case Variant::type_integer:
                    m_value = Variant( m_value.get_integer() - inputValue.get_integer() );
                    break;
                case Variant::type_real:
                    m_value = Variant( m_value.get_real() - inputValue.get_real() );
                    break;
                default:
                    VMF_EXCEPTION( vmf::NotImplementedException, "Operation not applicable to this data type" );
                }
                break;
            }
            return true;
        }
    virtual const Variant& value() const
        { return m_value; }

private:
    Variant m_value;

public:
    static StatOpBase* createInstance()
        { return new StatOpSum(); }
};

class StatOpLastVal: public StatOpBase
{
public:
    StatOpLastVal()
        {}
    virtual ~StatOpLastVal()
        {}

public:
    virtual std::string name() const
        { return StatOpFactory::builtinName(StatOpFactory::BuiltinOp::LastVal); }
    virtual void reset()
        { m_value = Variant(); }
    virtual bool handle( StatAction::Type action, const Variant& inputValue )
        {
            switch( action )
            {
            case StatAction::Add:
                m_value = inputValue;
                break;
            case StatAction::Remove:
                return false;
            }
            return true;
        }
    virtual const Variant& value() const
        { return m_value; }

private:
    Variant m_value;

public:
    static StatOpBase* createInstance()
        { return new StatOpLastVal(); }
};

// class StatOpFactory

StatOpBase* StatOpFactory::create( const std::string& name )
{
    // TODO: use lock for thread-safe access to UserOpMap instance
    const UserOpMap& ops = getClassMap();

    auto it = ops.find( name );
    if( it == ops.end() )
    {
        VMF_EXCEPTION( vmf::NotFoundException, "User operation not registered: '" + name + "'" );
    }

    StatOpBase* op = (it->second)();
    if( op == nullptr )
    {
        VMF_EXCEPTION( vmf::NullPointerException, "User operation isn't created: '" + name + "'" );
    }

    return op;
}

void StatOpFactory::registerUserOp( InstanceCreator createInstance )
{
    if( createInstance == nullptr )
    {
        VMF_EXCEPTION( vmf::NullPointerException, "Null pointer to the class instance creator for a user operation" );
    }

    std::unique_ptr<StatOpBase> userOp( createInstance() );
    std::string userOpName = userOp->name();

    // TODO: use lock for thread-safe access to UserOpMap instance
    UserOpMap& ops = getClassMap();

    auto it = ops.find(userOpName);
    if( it != ops.end() )
    {
        if( it->second != createInstance )
        {
            VMF_EXCEPTION(vmf::IncorrectParamException, "User operation is registered twice: '" + userOpName + "'");
        }
        else
        {
            // Registered twice with the same createInstance() function; it's not an error
        }
    }
    else
    {
        ops.insert( UserOpItem(userOpName, createInstance) );
    }
}


#define ALL_BUILTIN_OPS   \
        OP_ITEM(Min);     \
        OP_ITEM(Max);     \
        OP_ITEM(Average); \
        OP_ITEM(Count);   \
        OP_ITEM(Sum);     \
        OP_ITEM(LastVal);

#ifdef OP_ITEM
  #undef OP_ITEM
#endif
#define OP_ITEM(x)  ops.insert( UserOpItem( builtinName(BuiltinOp::x), StatOp ## x ::createInstance ) )
StatOpFactory::UserOpMap& StatOpFactory::getClassMap()
{
    // Singletone model based on initialisation of function-local static object
    // Object initialized once on first time we have entered the function
    static UserOpMap ops;

    if( ops.empty() )
    {
        ALL_BUILTIN_OPS
    }

    return ops;
}
#undef OP_ITEM

#ifdef OP_ITEM
  #undef OP_ITEM
#endif
#define OP_ITEM(x) case BuiltinOp:: ## x:  return prefix + #x
/*static*/ std::string StatOpFactory::builtinName(BuiltinOp::Type op)
{
    static const std::string prefix = "com.intel.statistics.builtin_operation.";
    switch (op)
    {
        ALL_BUILTIN_OPS
    }
    VMF_EXCEPTION(vmf::IncorrectParamException, "Unknown enum value: " + std::to_string((int)op));
}
#undef OP_ITEM


// class StatField (StatFieldDesc)

class StatField::StatFieldDesc
{
public:
    StatFieldDesc(const std::string& name, const std::string& schemaName,
        const std::string& metadataName, const std::string& fieldName,
        const std::string& opName)
        : m_name(name), m_schemaName(schemaName), m_metadataName(metadataName),
        m_metadataDesc(nullptr), m_fieldName(fieldName), m_fieldDesc(),
        m_opName(opName), m_pMetadataStream(nullptr) {}
    explicit StatFieldDesc(const StatFieldDesc& other)
        : m_name(other.m_name), m_schemaName(other.m_schemaName), m_metadataName(other.m_metadataName),
        m_metadataDesc(nullptr), m_fieldName(other.m_fieldName), m_fieldDesc(),
        m_opName(other.m_opName), m_pMetadataStream(nullptr) {}
    explicit StatFieldDesc(StatFieldDesc&& other)
        : m_name(std::move(other.m_name)), m_schemaName(std::move(other.m_schemaName)),
        m_metadataName(std::move(other.m_metadataName)), m_metadataDesc(std::move(nullptr)),
        m_fieldName(std::move(other.m_fieldName)), m_fieldDesc(std::move(other.m_fieldDesc)),
        m_opName(std::move(other.m_opName)), m_pMetadataStream(nullptr) {}
    StatFieldDesc()
        : m_name(""), m_schemaName(""), m_metadataName(""),
        m_metadataDesc(nullptr), m_fieldName(""), m_fieldDesc(),
        m_opName(""), m_pMetadataStream(nullptr) {}
    ~StatFieldDesc() {}

    StatFieldDesc& operator=(const StatFieldDesc& other);
    StatFieldDesc& operator=(StatFieldDesc&& other);

    const std::string& getName() const { return m_name; }
    const std::string& getSchemaName() const { return m_schemaName; }
    const std::string& getMetadataName() const { return m_metadataName; }
    std::shared_ptr< MetadataDesc > getMetadataDesc() const { return m_metadataDesc; }
    const std::string& getFieldName() const { return m_fieldDesc.name; }
    const std::string& getOpName() const { return m_opName; }

public:
    void setStream(MetadataStream* pMetadataStream);
    MetadataStream* getStream() const { return m_pMetadataStream; }

private:
    std::string m_name;
    std::string m_schemaName;
    std::string m_metadataName;
    std::shared_ptr< MetadataDesc > m_metadataDesc;
    std::string m_fieldName;
    FieldDesc m_fieldDesc;
    std::string m_opName;
    MetadataStream* m_pMetadataStream;
};

std::string StatField::getName() const
{ 
    return m_desc->getName();
}

std::shared_ptr< MetadataDesc > StatField::getMetadataDesc() const
{
    return m_desc->getMetadataDesc();
}

std::string StatField::getFieldName() const
{
    return m_desc->getFieldName();
}

std::string StatField::getOpName() const
{
    return m_desc->getOpName();
}

MetadataStream* StatField::getStream() const
{
    return m_desc->getStream();
}

StatField::StatField(
        const std::string& name,
        const std::string& schemaName,
        const std::string& metadataName,
        const std::string& fieldName,
        const std::string& opName )
    : m_desc( new StatFieldDesc( name, schemaName, metadataName, fieldName, opName ) )
    , m_op( StatOpFactory::create( opName ))
    , m_state( StatState::UpToDate )
    , m_isActive( false )
{
}

StatField::StatField( const StatField& other )
    : m_desc( new StatFieldDesc( *other.m_desc) )
    , m_op( (other.m_op != nullptr) ? StatOpFactory::create( other.m_op->name() ) : nullptr )
    , m_state( other.m_state )
    , m_isActive( other.m_isActive )
{
}

StatField::StatField( StatField&& other )
    : m_desc( std::move( other.m_desc ))
    , m_op( nullptr )
    , m_state( std::move( other.m_state ))
    , m_isActive( std::move( other.m_isActive ))
{
    std::swap( m_op, other.m_op );
}

StatField::StatField()
    : m_desc()
    , m_op( nullptr )
    , m_state( StatState::UpToDate )
    , m_isActive( false )
{
}

StatField::~StatField()
{
    delete m_op; m_op = nullptr;
}

StatField::StatFieldDesc& StatField::StatFieldDesc::operator=( const StatField::StatFieldDesc& other )
{
    setStream( nullptr );

    m_name         = other.m_name;
    m_schemaName   = other.m_schemaName;
    m_metadataName = other.m_metadataName;
    m_metadataDesc = nullptr;
    m_fieldName    = other.m_fieldName;
    m_fieldDesc    = FieldDesc();
    m_opName       = other.m_opName;

    setStream( other.getStream() );

    return *this;
}

StatField::StatFieldDesc& StatField::StatFieldDesc::operator=( StatField::StatFieldDesc&& other )
{
    setStream( nullptr );

    m_name         = std::move( other.m_name );
    m_schemaName   = std::move( other.m_schemaName );
    m_metadataName = std::move( other.m_metadataName );
    m_metadataDesc = std::move( other.m_metadataDesc );
    m_fieldName    = std::move( other.m_fieldName );
    m_fieldDesc    = std::move( other.m_fieldDesc );
    m_opName       = std::move( other.m_opName );

    setStream( other.getStream() );

    return *this;
}

StatField& StatField::operator=( const StatField& other )
{
    setStream( nullptr );

    m_desc.reset( new StatFieldDesc( *other.m_desc ) );

    delete m_op;
    m_op = (other.m_op != nullptr) ? StatOpFactory::create( other.m_op->name() ) : nullptr;

    m_state = other.m_state;
    m_isActive = other.m_isActive;

    setStream( other.getStream() );

    return *this;
}

StatField& StatField::operator=( StatField&& other )
{
    setStream( nullptr );

    m_desc = std::move( other.m_desc );

    delete m_op;
    m_op = nullptr;
    std::swap( m_op, other.m_op );

    m_state = other.m_state;
    m_isActive = other.m_isActive;

    setStream( other.getStream() );

    return *this;
}

StatState::Type StatField::handle( std::shared_ptr< Metadata > metadata )
{
    const std::shared_ptr< MetadataDesc > metadataDesc = metadata->getDesc();
    if( metadataDesc && (this->getMetadataDesc() == metadataDesc ))
    {
        const std::string& fieldName = this->getFieldName();
        Metadata::iterator it = metadata->findField( fieldName );
        if( it != metadata->end() )
        {
            updateState( m_op->handle( StatAction::Add, *it ));
        }
    }
    return getState();
}

void StatField::update( bool doRescan )
{
    if( isActive() && (doRescan || (getState() != StatState::UpToDate)) )
    {
        m_op->reset();
        MetadataSet metadataSet = getStream()->getAll();
        for( auto metadata : metadataSet )
        {
            handle( metadata );
        }
        m_state = StatState::UpToDate;
    }
}

void StatField::updateState( bool didUpdate )
{
    if( !didUpdate )
    {
        m_state = StatState::NeedRescan;
    }
}

void StatField::StatFieldDesc::setStream( MetadataStream* pMetadataStream )
{
    m_pMetadataStream = pMetadataStream;
    if( m_pMetadataStream != nullptr )
    {
        std::shared_ptr< MetadataSchema > schema = m_pMetadataStream->getSchema( m_schemaName );
        if( schema == nullptr )
        {
            VMF_EXCEPTION( IncorrectParamException, "Unknown schema '" + m_schemaName + "'" );
        }

        m_metadataDesc = schema->findMetadataDesc( m_metadataName );
        if( m_metadataDesc == nullptr )
        {
            VMF_EXCEPTION( IncorrectParamException, "Unknown metadata '" + m_metadataName + "' for schema '" + m_schemaName + "'" );
        }

        if( m_metadataDesc->getFieldDesc( m_fieldDesc, m_fieldName ) != true )
        {
            VMF_EXCEPTION( IncorrectParamException, "Unknown field '" + m_fieldName + "' for metadata '" + m_metadataName + "' for schema '" + m_schemaName + "'" );
        }
    }
    else
    {
        m_metadataDesc = nullptr;
        m_fieldDesc = FieldDesc();
    }
}

void StatField::setStream( MetadataStream* pMetadataStream )
{
    m_desc->setStream( pMetadataStream );
    m_isActive = bool( pMetadataStream != nullptr );
}

// class Stat (StatDesc, StatWorker)

class Stat::StatWorker
{
public:
    explicit StatWorker(Stat* stat)
        : m_stat(stat)
        , m_wakeupForced(false)
        , m_updateScheduled(false)
        , m_rescanScheduled(false)
        , m_exitScheduled(false)
        , m_exitImmediate(false)
    {
        m_worker = std::thread(&StatWorker::operator(), this);
    }
    ~StatWorker()
    {
        scheduleExit();
        m_worker.join();
        m_stat = nullptr;
    }
    void operator()()
    {
        // worker is starting
        for (;;)
        {
            // worker is going to sleep
            {
                std::unique_lock< std::mutex > lock(m_lock);
                if (m_stat->getUpdateMode() == StatUpdateMode::OnTimer)
                {
                    const unsigned tmo = std::max(m_stat->getUpdateTimeout(), (unsigned)10);
                    bool awaken = false;
                    do {
                        awaken = m_signal.wait_for(lock, std::chrono::milliseconds(tmo), [&]
                        {
                            return m_exitScheduled ||
                                m_rescanScheduled ||
                                m_wakeupForced ||
                                (m_updateScheduled && !m_items.empty());
                        });
                    } while (!awaken);
                }
                else
                {
                    m_signal.wait(lock, [&]
                    {
                        return m_exitScheduled ||
                            m_rescanScheduled ||
                            m_wakeupForced ||
                            (m_updateScheduled && !m_items.empty());
                    });
                }
                m_wakeupForced = false;
                if (m_exitScheduled && m_exitImmediate)
                    break;
            }
            // worker has awaken
            if (m_rescanScheduled)
            {
                // rescan
                if (m_stat != nullptr)
                    m_stat->rescan();
                {
                    std::unique_lock< std::mutex > lock(m_lock);
                    m_rescanScheduled = false;
                }
                if (m_stat != nullptr)
                    m_stat->resetState();
            }
            else if (m_updateScheduled)
            {
                std::shared_ptr< Metadata > metadata;
                while (tryPop(metadata))
                {
                    // processing of item
                    if (m_stat != nullptr)
                        m_stat->handle(metadata);
                }
                        {
                            std::unique_lock< std::mutex > lock(m_lock);
                            m_updateScheduled = false;
                        }
                        if (m_stat != nullptr)
                            m_stat->resetState();
            }
                    {
                        std::unique_lock< std::mutex > lock(m_lock);
                        if (m_exitScheduled && !m_exitImmediate)
                            break;
                    }
        }
        // worker is finishing
    }
    void scheduleUpdate(const std::shared_ptr< Metadata > val, bool doWake = true)
    {
        std::unique_lock< std::mutex > lock(m_lock);
        m_items.push(val);
        if (doWake && !m_updateScheduled && !m_items.empty())
        {
            m_updateScheduled = true;
            m_signal.notify_one();
        }
    }
    void scheduleRescan(bool doWake = true)
    {
        std::unique_lock< std::mutex > lock(m_lock);
        if (doWake && !m_rescanScheduled)
        {
            m_rescanScheduled = true;
            m_signal.notify_one();
        }
    }
    void scheduleExit(bool doImmediate = false)
    {
        std::unique_lock< std::mutex > lock(m_lock);
        if (!m_exitScheduled)
        {
            m_exitScheduled = true;
            m_exitImmediate = doImmediate;
            m_signal.notify_one();
        }
    }
    void wakeup(bool doForceWakeup = false)
    {
        std::unique_lock< std::mutex > lock(m_lock);
        m_updateScheduled = (m_updateScheduled || !m_items.empty());
        m_wakeupForced = doForceWakeup;
        if (m_exitScheduled || m_updateScheduled | m_wakeupForced)
        {
            m_signal.notify_one();
        }
    }
    void reset()
    {
        std::unique_lock< std::mutex > lock(m_lock);
        if (!m_items.empty())
            std::queue< std::shared_ptr< Metadata >>().swap(m_items);
        m_updateScheduled = false;
        m_rescanScheduled = false;
        m_exitScheduled = false;
        m_exitImmediate = false;
    }

private:
    bool tryPop(std::shared_ptr< Metadata >& metadata)
    {
        std::unique_lock< std::mutex > lock(m_lock);
        if (!m_items.empty()) {
            metadata = m_items.front();
            m_items.pop();
            return true;
        }
        m_updateScheduled = false;
        return false;
    }

private:
    Stat* m_stat;
    std::thread m_worker;
    std::queue< std::shared_ptr< Metadata >> m_items;
    std::atomic< bool > m_wakeupForced;
    std::atomic< bool > m_updateScheduled;
    std::atomic< bool > m_rescanScheduled;
    std::atomic< bool > m_exitScheduled;
    std::atomic< bool > m_exitImmediate;
    std::condition_variable m_signal;
    std::mutex m_lock;
};

class Stat::StatDesc
{
public:
    explicit StatDesc(const std::string& name) : m_name(name) {}
    explicit StatDesc(const StatDesc& other) : m_name(other.m_name) {}
    explicit StatDesc(StatDesc&& other) : m_name(std::move(other.m_name)) {}
    ~StatDesc() {}

    StatDesc& operator=(const StatDesc& other) { m_name = other.m_name; return *this; }
    StatDesc& operator=(StatDesc&& other) { m_name = std::move(other.m_name); return *this; }

    const std::string& getName() const { return m_name; }

private:
    std::string m_name;
};


Stat::Stat( const std::string& name, const std::vector< StatField >& fields, StatUpdateMode::Type updateMode )
    : m_desc( new StatDesc(name) )
    , m_fields( fields )
    , m_worker( new StatWorker(this) )
    , m_updateMode( updateMode )
    , m_state( StatState::UpToDate )
    , m_isActive( false )
{
}

Stat::Stat( const Stat& other )
    : m_desc( new StatDesc(*other.m_desc) )
    , m_fields( other.m_fields )
    , m_worker( new StatWorker(this) )
    , m_updateMode( other.m_updateMode )
    , m_state( other.m_state )
    , m_isActive( other.m_isActive )
{
}

Stat::Stat( Stat&& other )
    : m_desc( std::move( other.m_desc ))
    , m_fields( std::move( other.m_fields ))
    , m_worker(new StatWorker(this))
    , m_updateMode( other.m_updateMode )
    , m_state( other.m_state )
    , m_isActive( other.m_isActive )
{
}

Stat::~Stat()
{
}

Stat& Stat::operator=( const Stat& other )
{
    m_worker->reset();

    setStream( nullptr );
    m_desc.reset( new StatDesc(*other.m_desc) );
    m_fields     = other.m_fields;
    m_updateMode = other.m_updateMode;
    m_state      = other.m_state;
    m_isActive   = other.m_isActive;
    setStream( other.getStream() );

    return *this;
}

Stat& Stat::operator=( Stat&& other )
{
    m_worker->reset();

    setStream( nullptr );
    m_desc       = std::move( other.m_desc );
    m_fields     = std::move( other.m_fields );
    m_updateMode = std::move( other.m_updateMode );
    m_state      = std::move( other.m_state );
    m_isActive   = std::move( other.m_isActive );
    setStream( other.getStream() );

    return *this;
}

std::string Stat::getName() const
{
    return m_desc->getName();
}

void Stat::notify( StatAction::Type action, std::shared_ptr< Metadata > metadata )
{
    if( isActive() )
    {
        switch( action )
        {
        case StatAction::Add:
            switch( m_updateMode )
            {
            case StatUpdateMode::Disabled:
                break;
            case StatUpdateMode::Manual:
                m_state = StatState::NeedUpdate;
                m_worker->scheduleUpdate( metadata, false );
                break;
            case StatUpdateMode::OnAdd:
            case StatUpdateMode::OnTimer:
                m_state = StatState::NeedUpdate;
                m_worker->scheduleUpdate( metadata, true );
                break;
            }
            break;
        case StatAction::Remove:
            switch( m_updateMode )
            {
            case StatUpdateMode::Disabled:
                break;
            case StatUpdateMode::Manual:
                m_state = StatState::NeedRescan;
                break;
            case StatUpdateMode::OnAdd:
            case StatUpdateMode::OnTimer:
                m_state = StatState::NeedRescan;
                m_worker->scheduleRescan();
                break;
            }
            break;
        }
    }
}

void Stat::update( bool doRescan, bool doWait )
{
    if( isActive() && ((getState() != StatState::UpToDate) || doRescan) )
    {
        switch( m_updateMode )
        {
        case StatUpdateMode::Disabled:
            break;
        case StatUpdateMode::Manual:
        case StatUpdateMode::OnAdd:
        case StatUpdateMode::OnTimer:
            if( doRescan )
            {
                m_state = StatState::NeedRescan;
                m_worker->scheduleRescan();
            }
            else
            {
                m_state = StatState::NeedUpdate;
                m_worker->wakeup();
            }
            break;
        }
        if( doWait )
        {
            /*
            // TODO: Busy wait loop. Perhaps it would be better to wait on conditional variable.
            //       Also, it can be potentially dangerous and leads to deadlocks.
            //----
            //----
            */
            while( m_state != StatState::UpToDate )
                ;
        }
    }
}

void Stat::handle( const std::shared_ptr< Metadata > metadata )
{
    for( auto& statField : m_fields )
    {
        statField.handle( metadata );
    }
}

void Stat::rescan()
{
    for( auto& statField : m_fields )
    {
        statField.update( true );
    }
}

void Stat::setUpdateMode( StatUpdateMode::Type updateMode )
{
    if( updateMode != m_updateMode )
    {
        switch( updateMode )
        {
        case StatUpdateMode::Disabled:
            m_updateMode = updateMode;
            break;
        case StatUpdateMode::Manual:
        case StatUpdateMode::OnAdd:
        case StatUpdateMode::OnTimer:
            m_updateMode = updateMode;
            m_worker->wakeup( true );
            break;
        default:
            VMF_EXCEPTION( vmf::NotImplementedException, "Unknown update mode" );
        }
    }
}

std::vector< std::string > Stat::getAllFieldNames() const
{
    std::vector< std::string > names;

    for( auto& statField : m_fields )
    {
        names.push_back( statField.getName() );
    }

    return names;
}

const StatField& Stat::getField( const std::string& name ) const
{
    auto it = std::find_if( m_fields.begin(), m_fields.end(), [&]( const StatField& statField )->bool
    {
        return statField.getName() == name;
    });

    if( it == m_fields.end() )
    {
        VMF_EXCEPTION( vmf::NotFoundException, "Statistics field not found: '" + name + "'" );
    }

    return *it;
}

void Stat::setStream( MetadataStream* pMetadataStream )
{
    for( auto& statField : m_fields )
    {
        statField.setStream( pMetadataStream );
    }
    m_isActive = bool( pMetadataStream != nullptr );
}

MetadataStream* Stat::getStream() const
{
    return (m_fields.empty() ? nullptr : m_fields[0].getStream());
}

} // namespace vmf

