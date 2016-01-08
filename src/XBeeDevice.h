/**
 * Class for communicating with XBee devices.
 *
 * (C) 2014-2016 Stefan Marks
 * Auckland University of Technology
 */

#pragma once

#include "SerialPort.h"
#include "XBeePacket.h"


// forward declarations
class XBeeRemoteDevice;

/**
 * Base class for an XBee device with the basic information like address, serial#, and name.
 */
class XBeeDevice
{
public:

	/**
	 * Default constructor for an XBee device.
	 */
	XBeeDevice();

	/**
	 * Default destructor for an XBee device
	 */
	virtual ~XBeeDevice() { };

	/**
	 * Checks if the XBee device data is valid.
	 *
	 * @return <code>true</code> if data is valid.
	 */
	bool isValid() const;

	/**
	 * Gets the serial number (=64 Bit network MAC address) of the device.
	 *
	 * @return the serial number of the device
	 */
	uint64_t getSerialNumber() const;

	/**
	 * Gets the 16 bit network address of the device.
	 *
	 * @return the network address of the device
	 */
	uint16_t getNetworkAddress() const;

	/**
	 * Gets the name of the device (if defined).
	 *
	 * @return the name of the device
	 */
	std::string getName() const;


	/**
	 * Gets the software version of the device.
	 *
	 * @return the software version of the device
	 */
	uint16_t getSoftwareVersion() const;

	/**
	 * Gets the hardware version of the device.
	 *
	 * @return the hardware version of the device
	 */
	uint16_t getHardwareVersion() const;

protected:

	uint64_t    m_serialNumber;
	uint16_t    m_networkAddress;
	std::string m_strName;
	uint16_t    m_versionHW;
	uint16_t    m_versionSW;

};



/**
 * Class for an XBee coordinator device that is connected to the host via a serial port.
 */
class XBeeCoordinator : public XBeeDevice
{
public:

	/**
	 * Creates a XBee coordinator device class for a specific serial port.
	 * The port is opened and set to a default baudrate of 57600.
	 *
	 * @param refPort  the serial port to use
	 */
	XBeeCoordinator(SerialPort& refPort);

	/**
	 * Default destructor.
	 * Closes the serial port.
	 */
	virtual ~XBeeCoordinator();

	/**
	 * Sends a packet to an XBee device.
	 *
	 * @param refSend  the packet to send
	 *
	 * @return <code>true</code> if sending was successful,
	 *         <code>false</code> if not
	 */
	bool send(XBeePacket_Send& refSend);

	/**
	 * Receives a specific packet from an XBee device.
	 *
	 * @param refReceive  the packet to receive
	 *
	 * @return <code>true</code> if receiving was successful,
	 *         <code>false</code> if not
	 */
	bool receive(XBeePacket_Receive& refReceive);

	/**
	 * Receives an unspecific packet from an XBee device.
	 *
	 * @return received packet or <code>NULL</code> if an error occured
	 */
	std::unique_ptr<XBeePacket_Receive> receive();

	/**
	 * Sends a packet to an XBee device and waits for the reply.
	 *
	 * @param refSend     the packet to send
	 * @param refReceive  the packet to receive
	 *
	 * @return <code>true</code> if the process was succesful
	 */
	bool process(XBeePacket_Send& refSend, XBeePacket_Receive& refReceive);

	/**
	 * Sets the number of retries when a received packet is not as expected.
	 *
	 * @param retries  the number of retries
	 */
	void setNumberOfRetries(int retries);

	/**
	 * Scans for any other connected devices.
	 *
	 * @return  number of connected devices found
	 */
	int scanDevices();

	/**
	 * Gets the list of connected devices.
	 *
	 * @return  the list of connected devices
	 */
	const std::vector<XBeeRemoteDevice*>& getConnectedDevices() const;


protected:

	/**
	 * Receives a specific packet from an XBee device.
	 *
	 * @return <code>true</code> if reception was successful and fata is in the buffer
	 */
	bool receivePacket();

protected:

	SerialPort&      m_serialPort;   // the serial port to use for this device
	uint8_t          m_frameCounter; // current frame ID for command/response pairs
	int              m_numOfRetries; // the number of receive retries
	XBeeReadBuffer   m_bufIn;        // buffer for incoming data
	XBeeWriteBuffer  m_bufOut;       // buffer for outgoing data

	std::vector<XBeeRemoteDevice*> m_arrNodes;  // connected XBee nodes

};



/**
 * Class for an XBee device that is connected wirelessly via a coordinator.
 */
class XBeeRemoteDevice : public XBeeDevice
{
public:

	enum DeviceType
	{
		Coordinator = 0x00,
		Router      = 0x01,
		EndDevice   = 0x02
	};

public:

	/**
	 * Creates a XBee remote device class for a specific coordinator.
	 *
	 * @param refCoordinator  the coordinator for this device
	 * @param refBuffer       the buffer to extract information from (generated by a "ND" command)
	 */
	XBeeRemoteDevice(XBeeCoordinator& refCoordinator, const XBeeReadBuffer& refBuffer);

	virtual ~XBeeRemoteDevice() { };

	/**
	 * Gets the network address of the parent.
	 *
	 * @return  the network address of the parent
	 */
	uint16_t getParentAddress() const;

	/**
	 * Gets the type of the remote device.
	 *
	 * @return  the type of the remote device (e.g., Router, Coordinator)
	 */
	DeviceType getDeviceType() const;

protected:

	XBeeCoordinator&  m_coordinator;
	uint16_t          m_parentAddress;
	DeviceType        m_deviceType;
};

