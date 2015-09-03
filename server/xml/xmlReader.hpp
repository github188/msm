#ifndef XMLREADER_HPP
#define XMLREADER_HPP

class XmlReader
{
public:
    XmlReader(const char *filename = "./test.xml"): m_filename(filename){}

    void xmlReadTitleInfo();
    void xmlReadSelfInfo();
    void xmlReadContactInfo();
    void xmlReadMeetingInfo();
    void xmlReadMsmServerInfo();
    void xmlReadAll();

private:
    const char * m_filename;
};


#endif
