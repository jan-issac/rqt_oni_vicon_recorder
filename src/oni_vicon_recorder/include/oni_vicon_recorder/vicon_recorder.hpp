
#ifndef ONI_VICON_RECORDER_VICON_RECORDER_HPP
#define ONI_VICON_RECORDER_VICON_RECORDER_HPP

#include <iostream>
#include <fstream>
#include <cassert>
#include <ctime>
#include <iomanip>
#include <set>
#include <vector>

#include <boost/thread/shared_mutex.hpp>

#include <vicon_sdk/vicon_client.h>

#include <actionlib/server/simple_action_server.h>

#include <oni_vicon_recorder/ConnectToViconAction.h>
#include <oni_vicon_recorder/ViconObjects.h>
#include <oni_vicon_recorder/VerifyObjectExists.h>
#include <oni_vicon_recorder/VerifyObjectExists.h>
#include <oni_vicon_recorder/ViconFrame.h>

#include "oni_vicon_recorder/frame_time_tracker.hpp"

/**
 * @class ViconRecorder records a subset of the vicon data
 *
 * Recorded file format
 *
 *  Each line contains data of a single frame. Each line is build up as follows
 *
 *  - (unsigned int) Recording frame number (starting from zero)
 *  - (unsigned int) FrameNumber
 *  - (unsigned int) Output_GetTimecode.Hours;
 *  - (unsigned int) Output_GetTimecode.Minutes;
 *  - (unsigned int) Output_GetTimecode.Seconds;
 *  - (unsigned int) Output_GetTimecode.Frames;
 *  - (unsigned int) Output_GetTimecode.SubFrame;
 *  - (TimecodeStandard::Enum) Output_GetTimecode.Standard;
 *  - (unsigned int) Output_GetTimecode.SubFramesPerFrame;
 *  - (unsigned int) Output_GetTimecode.UserBits;
 *
 *  - (double) Output_GetSegmentGlobalTranslation.Translation[ 0 ]
 *  - (double) Output_GetSegmentGlobalTranslation.Translation[ 1 ]
 *  - (double) Output_GetSegmentGlobalTranslation.Translation[ 2 ]
 *
 *  - (double) Output_GetSegmentGlobalRotationMatrix.Rotation[ 0 ]
 *  - (double) Output_GetSegmentGlobalRotationMatrix.Rotation[ 1 ]
 *  - (double) Output_GetSegmentGlobalRotationMatrix.Rotation[ 2 ]
 *  - (double) Output_GetSegmentGlobalRotationMatrix.Rotation[ 3 ]
 *  - (double) Output_GetSegmentGlobalRotationMatrix.Rotation[ 4 ]
 *  - (double) Output_GetSegmentGlobalRotationMatrix.Rotation[ 5 ]
 *  - (double) Output_GetSegmentGlobalRotationMatrix.Rotation[ 6 ]
 *  - (double) Output_GetSegmentGlobalRotationMatrix.Rotation[ 7 ]
 *  - (double) Output_GetSegmentGlobalRotationMatrix.Rotation[ 8 ]
 *
 *  - (double) Output_GetSegmentGlobalRotationQuaternion.Rotation[ 0 ]
 *  - (double) Output_GetSegmentGlobalRotationQuaternion.Rotation[ 1 ]
 *  - (double) Output_GetSegmentGlobalRotationQuaternion.Rotation[ 2 ]
 *  - (double) Output_GetSegmentGlobalRotationQuaternion.Rotation[ 3 ]
 *
 *  - (double) Output_GetSegmentGlobalRotationEulerXYZ.Rotation[ 0 ]
 *  - (double) Output_GetSegmentGlobalRotationEulerXYZ.Rotation[ 1 ]
 *  - (double) Output_GetSegmentGlobalRotationEulerXYZ.Rotation[ 2 ]
 *  "\n"
 */
class ViconRecorderStub
{
public:
    ViconRecorderStub(ros::NodeHandle& node_handle, int float_precision = 5);
    ~ViconRecorderStub();

public: /* Action Callbacks */
    void connectCB(const oni_vicon_recorder::ConnectToViconGoalConstPtr& goal);

public: /* Service Callbacks */
    bool viconObjectsCB(oni_vicon_recorder::ViconObjects::Request& request,
                        oni_vicon_recorder::ViconObjects::Response& response);
    bool objectExistsCB(oni_vicon_recorder::VerifyObjectExists::Request& request,
                        oni_vicon_recorder::VerifyObjectExists::Response& response);
private:
    int float_precision_;
    bool connected_;
    boost::shared_mutex iteration_mutex_;

    std::string hostname_;
    std::string multicast_address_;
    std::string object_;
    bool connect_to_multicast_;
    bool multicast_enabled_;

    actionlib::SimpleActionServer<
        oni_vicon_recorder::ConnectToViconAction> connect_to_vicon_as_;

    ros::ServiceServer vicon_objects_srv_;
    ros::ServiceServer object_verification_srv_;
};

class ViconRecorder
{
public:
    ViconRecorder(ros::NodeHandle& node_handle,
                  FrameTimeTracker::Ptr frame_time_tracker,
                  int float_precision = 5);
    ~ViconRecorder();

    bool startRecording(const std::string& file, const std::string& object_name);
    bool stopRecording();
    u_int64_t countFrames();
    bool isRecording();
    void closeConnection();

    std::ofstream& beginRecord(std::ofstream& ofs);
    std::ofstream& record(std::ofstream& ofs);
    std::ofstream& endRecord(std::ofstream& ofs);

    void record_stuff();


public: /* Action Callbacks */
    void connectCB(const oni_vicon_recorder::ConnectToViconGoalConstPtr& goal);

public: /* Service Callbacks */
    bool viconObjectsCB(oni_vicon_recorder::ViconObjects::Request& request,
                        oni_vicon_recorder::ViconObjects::Response& response);
    bool objectExistsCB(oni_vicon_recorder::VerifyObjectExists::Request& request,
                        oni_vicon_recorder::VerifyObjectExists::Response& response);

    bool viconFrame(oni_vicon_recorder::ViconFrame::Request& request,
                    oni_vicon_recorder::ViconFrame::Response& response);

private:
    /**
     * @brief getViconObject() Gets the set of defined objects/subjects in the vicon system
     *
     * @return objects set
     */
    std::set<std::string> getViconObject();

    /**
     * @brief recordFrame Records current Vicon frame
     */
    bool recordFrame();

private:
    int float_precision_;
    bool connected_;
    bool recording_;
    u_int64_t frames_;
    boost::shared_mutex iteration_mutex_;
    std::ofstream ofs_;
    std::string object_name_;

    std::string hostname_;
    std::string multicast_address_;
    std::string object_;
    bool connect_to_multicast_;
    bool multicast_enabled_;
    ViconDataStreamSDK::CPP::Client vicon_client_;

    actionlib::SimpleActionServer<
        oni_vicon_recorder::ConnectToViconAction> connect_to_vicon_as_;

    ros::ServiceServer vicon_objects_srv_;
    ros::ServiceServer vicon_frame_srv_;
    ros::ServiceServer object_verification_srv_;

    FrameTimeTracker::Ptr frame_time_tracker_;
};


#endif
