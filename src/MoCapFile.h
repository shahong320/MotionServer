/**
 * Motion Capture system for reading files and a class for writing files.
 */

#pragma once

#include "MoCapSystem.h"
#include "VectorMath.h"

#include <fstream>
#include <sstream>
#include <string>


/**
 * Interface for writing ints/floats/strings to a file.
 * The underlying implementation determines the format, e.g., text, binary.
 */
class IFileWriter
{
public:
	virtual bool open(const std::string& filename) = 0;
	virtual bool close() = 0;
	virtual bool isOK() = 0;
	virtual void writeInt(int iValue) = 0;
	virtual void writeFloat(float fValue) = 0;
	virtual void writeString(const char* czString) = 0;
	virtual void writeTag(const char* czString) = 0;
	virtual void nextLine() = 0;
};


/**
 * Class for writing MoCap data to a text file.
 */
class MoCapFileWriter 
{
public:

	/**
	 * Creates a MoCap data file writer.
	 *
	 * @param framerate  the frame rate of the data in Hz
	 */
	MoCapFileWriter(float framerate);

	/**
	 * Destroys the MoCap data file writer.
	 */
	~MoCapFileWriter();

public:

	/**
	 * Writes the scene description to the file.
	 * This only needs to happen once at the beginning. 
	 * If the function is called again, it closes any previously opened file
	 * and starts a new one.
	 *
	 * @param refData  the MoCap data to write
	 *
	 * @return <code>true</code> if the data was written successfully
	 */
	bool writeSceneDescription(const MoCapData& refData);

	/**
	 * Writes a single frame of data to the file.
	 *
	 * @param refData  the MoCap data to write
	 *
	 * @return <code>true</code> if the data was written successfully
	 */
	bool writeFrameData(const MoCapData& refData);

private:

	/**
	 * Opens a new data file with the current timestamp.
	 *
	 * @return <code>true</code> if the file was opened successfully
	 */
	bool openFile();

	/**
	 * Closes any currently opened file.
	 *
	 * @return <code>true</code> if the file was opened successfully
	 */
	bool closeFile();

	/**
	 * Creates a string with a timestamp filename in the format
	 * "MotionServer File YYYY_MM_DD_HH_MM.SS.mot".
	 *
	 *@return the timestamp filename
	 */
	std::string getTimestampFilename();

	void writeMarkerSetDescription( const sMarkerSetDescription&  descr);
	void writeRigidBodyDescription( const sRigidBodyDescription&  descr);
	void writeSkeletonDescription(  const sSkeletonDescription&   descr);
	void writeForcePlateDescription(const sForcePlateDescription& descr);

	void writeMarkerSetData( const sMarkerSetData&  data);
	void writeRigidBodyData( const sRigidBodyData&  data);
	void writeSkeletonData(  const sSkeletonData&   data);
	void writeForcePlateData(const sForcePlateData& data);

	void writeDelimiter();
	void write(int   iValue);
	void write(float fValue);
	void write(const char* czString);
	void writeTag(const char* czString);
	void nextLine();

private:

	float         updateRate;
	std::ofstream output;
	bool          headerWritten, lineStarted;
	int           lastFrame;
	char          czBuf[256];
};



/**
 * Class for reading MoCap data from a text file and acting like a live MoCap system.
 */
class MoCapFileReader : public MoCapSystem
{
public:

	/**
	 * Creates a MoCap data file reader.
	 *
	 * @param strFilename  the filename of the MoCap data file to read
	 */
	MoCapFileReader(const std::string& strFilename);

	/**
	 * Destroys the MoCap data file reader.
	 */
	~MoCapFileReader();

public:
	virtual bool  initialise();
	virtual bool  isActive();
	virtual float getUpdateRate();
	virtual bool  update();
	virtual bool  getSceneDescription(MoCapData& refData);
	virtual bool  getFrameData(MoCapData& refData);
	virtual bool  processCommand(const std::string& strCommand);
	virtual bool  deinitialise();

private:

	/**
	 * Reads the header of the MoCap file and determines things like the version and the framerate.
	 *
	 * @return <code>true</code> if the header was read successfully
	 */
	bool readHeader();

	void readMarkerSetDescription( sMarkerSetDescription&  descr, sMarkerSetData&  data);
	void readRigidBodyDescription( sRigidBodyDescription&  descr, sRigidBodyData&  data);
	void readSkeletonDescription(  sSkeletonDescription&   descr, sSkeletonData&   data);
	void readForcePlateDescription(sForcePlateDescription& descr, sForcePlateData& data);

	void readMarkerSetData( sMarkerSetData&  data);
	void readRigidBodyData( sRigidBodyData&  data);
	void readSkeletonData(  sSkeletonData&   data);
	void readForcePlateData(sForcePlateData& data);

	void        nextLine();
	void        skipDelimiter();
	int         readInt();
	int         readInt(int min, int max);
	float       readFloat();
	const char* readString();
	bool        readTag(const char* czString);

private:

	std::string        strFilename;
	int                fileVersion;
	float              updateRate;

	std::ifstream      input;
	char*              pBuf;
	char               czBuf[256];
	const char*        pRead;
	int                bufSize;

	std::streampos     posDescriptions, posFrames;
	bool               fileOK, headerOK;
};

