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
/*
 * This demo shows how to use metadata descriptions
 * to store data structures such as GPS coordinates with associative time.
 */

#include <algorithm>
#include <iostream>
#include <cstdio>
#include <cassert>
#include <chrono>
#include <thread>

#include "vmf/vmf.hpp"

//using namespace vmf;
using namespace std;

string workingPath;

#define VIDEO_FILE (workingPath + "BlueSquare.avi")

void copyFile(const string& srcName, const char *dstName);

class StrCatOp: public vmf::StatOpBase
{
public:
    StrCatOp()
        {}
    virtual ~StrCatOp()
        {}

public:
    virtual const std::string& name() const
        { return opName(); }
    virtual void reset()
        { m_value = ""; }
    virtual bool handle( vmf::StatAction::Type action, const vmf::Variant& inputValue )
        {
            if( inputValue.getType() != vmf::Variant::type_string )
                VMF_EXCEPTION( vmf::NotImplementedException, "Operation not applicable to this data type" );
            switch( action )
            {
            case vmf::StatAction::Add:
                if( !m_value.empty() )
                    m_value += " | ";
                m_value += inputValue.get_string();
                break;
            case vmf::StatAction::Remove:
                return false;
            }
            return true;
        }
    virtual const vmf::Variant& value() const
        { m_temp = vmf::Variant( (vmf::vmf_string)m_value ); return m_temp; }

private:
    std::string m_value;
    mutable vmf::Variant m_temp; // temp return value for getValue()

public:
    static StatOpBase* createInstance()
        { return new StrCatOp(); }
    static const std::string& opName()
        {
            static const std::string name( "User.StrCatOp" );
            return name;
        }
};

inline std::ostream& operator<<( std::ostream& os, const vmf::Variant& value )
{
    os << "(" << value.getTypeName() << ") " << value.toString();
    return os;
}

static void dumpStatistics( const vmf::MetadataStream& mdStream )
{
    std::cout << std::endl;
    std::cout << "** statistics dump **" << std::endl;

    std::vector< std::string > statNames = mdStream.getAllStatNames();
    if( !statNames.empty() )
    {
        for( auto& statName : statNames )
        {
            const vmf::Stat& stat = mdStream.getStat( statName );
            std::cout << "statistics: '" << stat.getName() << "'" << std::endl;

            std::vector< std::string > fieldNames = stat.getAllFieldNames();
            if( !fieldNames.empty() )
            {
                for( auto& fieldName : fieldNames )
                {
                    const vmf::StatField& field = stat.getField( fieldName );

                    std::cout << "  name='" << field.getName()
                              << "'  metadata='" << field.getMetadataDesc()->getMetadataName()
                              << "'  field='" << field.getFieldName()
                              << "'  value=" << field.getValue()
                              << std::endl;
                }
            }
            else
            {
                std::cout << "  no field(s) defined" << std::endl;
            }
        }
    }
    else
    {
        std::cout << "There are statistics object(s) found" << std::endl;
    }

    std::cout << "** end of statistics dump **" << std::endl;
    std::cout << std::endl;
}

int sample(int argc, char *argv[])
{
    vmf::initialize();

    string appPath = argv[0];
#ifdef WIN32
    char delim = '\\';
#else
    char delim = '/';
#endif
    size_t pos = appPath.find_last_of(delim);

    if (pos != string::npos)
    {
        workingPath = appPath.substr(0, pos + 1);
    }

    // Copy test video file to another location.
    copyFile(VIDEO_FILE, "global_test.avi");

    cout << "VMF sample: read/write GPS location and time" << endl << endl;

    const string FILE_NAME = "global_test.avi";
    const string META_SOURCE_NAME = "test-id";
    const string GPS_DESC = "gps";
    const string GPS_COORD_FIELD = "GPS";
    const string GPS_TIME_FIELD = "Time";
    const string GPS_SCHEMA_NAME = "gps_schema";
    const string GPS_METADATA_ITEM1 = "lat=53.78,lng=132.46";
    const string GPS_METADATA_ITEM2 = "lat=53.28,lng=131.87";
    const string GPS_METADATA_ITEM3 = "lat=52.95,lng=131.41";
    const string GPS_METADATA_ITEM4 = "lat=52.49,lng=130.98";
    const string GPS_STAT_NAME = "gps_stat";
    const string GPS_COUNT_COORD_NAME = "coord count";
    const string GPS_COUNT_TIME_NAME = "time count";
    const string GPS_STRCAT_COORD_NAME = "coord strcat";
    const string GPS_STRCAT_TIME_NAME = "time strcat";

    cout << "Adding metadata..." << endl;
    cout << "Opening file name '" << FILE_NAME << "'" << endl;

    // Open metadata stream
    vmf::MetadataStream mdStream;
    if (!mdStream.open(FILE_NAME, vmf::MetadataStream::Update))
    {
        cerr << "Can't open file " << FILE_NAME << endl;
        exit(1);
    }

    // Create a GPS metadata field descriptions
    vector<vmf::FieldDesc> fieldDesc;

    fieldDesc.push_back(vmf::FieldDesc(GPS_COORD_FIELD)); // GPS coordinate as string
    fieldDesc.push_back(vmf::FieldDesc(GPS_TIME_FIELD)); // Associated time as string
    
    // Create GPS metadata description
    std::shared_ptr<vmf::MetadataDesc> gpsDesc = std::make_shared<vmf::MetadataDesc>(GPS_DESC, fieldDesc);

    // Create GPS metadata schema
    std::shared_ptr<vmf::MetadataSchema> gpsSchema = std::make_shared<vmf::MetadataSchema>(GPS_SCHEMA_NAME);

    // Add description to the schema
    gpsSchema->add(gpsDesc);

    string t = "21.02.2013 18:35";
    cout << "Add metadata schema '" << GPS_SCHEMA_NAME << "'" << endl;

    // Add schema to metadata stream
    mdStream.addSchema(gpsSchema);

    // Register user operation: exactly one of two calls must be issued once for each user op
    // vmf::StatOpFactory::registerUserOp( StrCatOp::opName(), StrCatOp::createInstance );
    vmf::StatOpFactory::registerUserOp< StrCatOp >();

    // Set up statistics object(s)
    std::vector< vmf::StatField > fields;
    fields.emplace_back( GPS_COUNT_COORD_NAME, GPS_SCHEMA_NAME, GPS_DESC, GPS_COORD_FIELD, vmf::StatOpFactory::countName() );
    fields.emplace_back( GPS_COUNT_TIME_NAME, GPS_SCHEMA_NAME, GPS_DESC, GPS_TIME_FIELD, vmf::StatOpFactory::countName() );
    fields.emplace_back( GPS_STRCAT_COORD_NAME, GPS_SCHEMA_NAME, GPS_DESC, GPS_COORD_FIELD, StrCatOp::opName() );
    fields.emplace_back( GPS_STRCAT_TIME_NAME, GPS_SCHEMA_NAME, GPS_DESC, GPS_TIME_FIELD, StrCatOp::opName() );
    mdStream.addStat( GPS_STAT_NAME, fields, vmf::StatUpdateMode::Disabled );
//    mdStream.addStat( GPS_STAT_NAME, fields, vmf::StatUpdateMode::Manual );
//    mdStream.addStat( GPS_STAT_NAME, fields, vmf::StatUpdateMode::OnAdd );
//    mdStream.addStat( GPS_STAT_NAME, fields, vmf::StatUpdateMode::OnTimer );

    mdStream.getStat(GPS_STAT_NAME).setUpdateTimeout( 50 );
    mdStream.getStat(GPS_STAT_NAME).setUpdateMode( vmf::StatUpdateMode::OnTimer );

    std::shared_ptr<vmf::Metadata> gpsMetadata;

    t = "21.02.2013 18:45";
    cout << "Adding metadata's item '" << GPS_METADATA_ITEM1 << "' with associated time " << t << endl;
    
    // Create a metadata item
    gpsMetadata = std::make_shared<vmf::Metadata>(gpsDesc);

    // Fill item fields
    gpsMetadata->push_back(vmf::FieldValue(GPS_COORD_FIELD, GPS_METADATA_ITEM1));
    gpsMetadata->push_back(vmf::FieldValue(GPS_TIME_FIELD, t));

    // Add to metadata a new item
    mdStream.add(gpsMetadata);

    t = "21.02.2013 19:28";
    cout << "Adding metadata's item '" << GPS_METADATA_ITEM2 << "' with associated time " << t << endl;

    gpsMetadata = std::make_shared<vmf::Metadata>(gpsDesc);
    gpsMetadata->push_back(vmf::FieldValue(GPS_COORD_FIELD, GPS_METADATA_ITEM2));
    gpsMetadata->push_back(vmf::FieldValue(GPS_TIME_FIELD, t));
    mdStream.add(gpsMetadata);

//    mdStream.getStat(GPS_STAT_NAME).update();

    t = "21.02.2013 21:02";
    cout << "Adding metadata's item '" << GPS_METADATA_ITEM3 << "' with associated time " << t << endl;

    gpsMetadata = std::make_shared<vmf::Metadata>(gpsDesc);
    gpsMetadata->push_back(vmf::FieldValue(GPS_COORD_FIELD, GPS_METADATA_ITEM3));
    gpsMetadata->push_back(vmf::FieldValue(GPS_TIME_FIELD, t));
    mdStream.add(gpsMetadata);

    t = "21.02.2013 23:19";
    cout << "Adding metadata's item '" << GPS_METADATA_ITEM4 << "' with associated time " << t << endl;

    gpsMetadata = std::make_shared<vmf::Metadata>(gpsDesc);
    gpsMetadata->push_back(vmf::FieldValue(GPS_COORD_FIELD, GPS_METADATA_ITEM4));
    gpsMetadata->push_back(vmf::FieldValue(GPS_TIME_FIELD, t));
    mdStream.add(gpsMetadata);

    mdStream.getStat(GPS_STAT_NAME).update();

    // dump statistics
    dumpStatistics( mdStream );

    cout << "Save metadata" << endl << endl;

    // Save metadata to video file and close metadata stream
    mdStream.save();
    mdStream.close();

    cout << "Loading metadata..." << endl;
    cout << "Opening file name '" << FILE_NAME << "'" << endl;
    
    // Open new metadata stream to load and print saved metadata
    vmf::MetadataStream loadStream;
    if (!loadStream.open(FILE_NAME, vmf::MetadataStream::ReadOnly))
    {
        cerr << "Can't open file " << FILE_NAME << endl;
        exit(1);
    }

    cout << "Loading schema '" << GPS_SCHEMA_NAME << "'" << endl;
    if (!loadStream.load(GPS_SCHEMA_NAME))
    {
        cerr << "Can't load schema " << GPS_SCHEMA_NAME << endl;
        exit(1);
    }

    loadStream.getStat(GPS_STAT_NAME).setUpdateMode( vmf::StatUpdateMode::Manual );
//    loadStream.getStat(GPS_STAT_NAME).update( true );

    // Select all metadata items from loaded schema
    auto dataSet = loadStream.queryBySchema(GPS_SCHEMA_NAME);

    // and print them to console
    for (size_t i = 0; i < dataSet.size(); i++)
    {
        cout << "Getting item " << i << endl;
        auto metadataItem = dataSet[i];

        string coord = metadataItem->getFieldValue(GPS_COORD_FIELD);
        cout << "\tGPS coordinates are: " << coord << endl;

        string time = metadataItem->getFieldValue(GPS_TIME_FIELD);
        cout << "\tAssociated time is: " << time << endl;
    }

    loadStream.getStat(GPS_STAT_NAME).update( true, true );

    // dump statistics
    dumpStatistics( loadStream );

    // Close metadata stream
    loadStream.close();

    // Uninitialize VMF library to free allocated resources
    vmf::terminate();

    return 0;
}

int main(int argc, char *argv[])
{
    try
    {
        return sample(argc, argv);
    }
    catch( const vmf::Exception& e )
    {
        std::cerr << "EXCEPTION : " << e.getMessage() << std::endl;
        return EXIT_FAILURE;
    }
}

