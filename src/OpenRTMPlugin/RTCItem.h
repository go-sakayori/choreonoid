/**
   @author 
*/

#ifndef CNOID_OPENRTM_PLUGIN_RTC_ITEM_H
#define CNOID_OPENRTM_PLUGIN_RTC_ITEM_H

#include <cnoid/Item>
#include <cnoid/Process>
#include <boost/filesystem.hpp>
#include <rtm/Manager.h>
#include "exportdecl.h"

namespace cnoid {

class MessageView;
typedef std::map<std::string, std::string> PropertyMap;

class RTComponent
{
public:
    RTComponent(const boost::filesystem::path& modulePath, PropertyMap& properties);
    ~RTComponent();
    void deleteRTC(bool waitToBeDeleted);
    RTC::RtcBase* rtc() { return rtc_; };
    bool isValid() const;
    const std::string& name() const { return componentName; }
        
private:
    RTC::RTObject_var rtcRef;
    RTC::RtcBase* rtc_;
    boost::filesystem::path modulePath;
    Process rtcProcess;
    std::string componentName;
    MessageView* mv;

    void init(const std::string& moduleName, PropertyMap& properties);
    void init(const boost::filesystem::path& modulePath, PropertyMap& properties);
    bool createRTC(PropertyMap& properties);
    void setupModules(std::string& fileName, std::string& initFuncName, std::string& componentName, PropertyMap& properties );
    void createProcess(std::string& command, PropertyMap& properties);
    void onReadyReadServerProcessOutput();
};

class CNOID_EXPORT RTCItem : public Item
{
public:
    static void initialize(ExtensionManager* ext);
        
    RTCItem();
    RTCItem(const RTCItem& org);
    virtual ~RTCItem();
    
    enum PERIODIC_TYPE {
        PERIODIC_EXECUTION_CONTEXT = 0,
        SYNCH_EXT_TRIGGER,
        EXT_TRIG_EXECUTION_CONTEXT,
        CHOREONOID_EXECUTION_CONTEXT,
        N_PERIODIC_TYPE
    };
    
    enum PathBase {
        RTC_DIRECTORY = 0,
        PROJECT_DIRECTORY,
        N_PATH_BASE
    };

    void setModuleName(const std::string& name);
    void setPeriodicType(int type);
    void setPeriodicRate(int rate);
    void setPathBase(int base);

protected:
    virtual void onPositionChanged();
    virtual void onDisconnectedFromRoot();
    virtual Item* doDuplicate() const;
    virtual void doPutProperties(PutPropertyFunction& putProperty);
    virtual bool store(Archive& archive);
    virtual bool restore(const Archive& archive);
        
private:
    std::ostream& os;
    MessageView* mv;
    std::string moduleName;
    RTComponent* rtcomp;
    Selection periodicType;
    int periodicRate;
    int oldType;
    PropertyMap properties;
    Selection pathBase;
    int oldPathBase;
    boost::filesystem::path modulePath;

    bool convertAbsolutePath();
};
        
typedef ref_ptr<RTCItem> RTCItemPtr;

}

#endif
