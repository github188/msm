#include "xmlReader.hpp"
#include "tinyxml2.h"

using namespace tinyxml2;

void XmlReader::xmlReadTitleInfo(void)
{
    XMLDocument doc;
    doc.LoadFile(m_filename);
    if (doc.ErrorID() != 0) {
        printf("read xml error!\n");
        return;
    }
    XMLElement *root = doc.FirstChildElement("root");

    XMLElement *dev_id = root->FirstChildElement("dev_id");
    XMLElement *user_account = root->FirstChildElement("user_acount");
    XMLElement *vsp_url = root->FirstChildElement("vsp_url");
    XMLElement *code = root->FirstChildElement("code");
    XMLElement *msg = root->FirstChildElement("msg");

    printf("dev_id: %s\n", dev_id->GetText());
    printf("user_account: %s\n", user_account->GetText());
    printf("vsp_url: %s\n", vsp_url->GetText());;
    printf("code: %s\n", code->GetText());
    printf("msg: %s\n", msg->GetText());
    printf("============================================\n");
}
void XmlReader::xmlReadSelfInfo(void)
{
    XMLDocument doc;
    doc.LoadFile(m_filename);
    if (doc.ErrorID() != 0) {
        printf("read xml error!\n");
        return;
    }
    XMLElement *root = doc.FirstChildElement("root");

    XMLElement *user = root->FirstChildElement("user");
    XMLElement *user_id = user->FirstChildElement("user_id");
    XMLElement *user1_account = user->FirstChildElement("user_acount");
    XMLElement *user_name = user->FirstChildElement("user_name");
    XMLElement *big_visit_num = user->FirstChildElement("big_visit_num");
    XMLElement *vsp_id = user->FirstChildElement("vsp_id");
    XMLElement *vsp_name = user->FirstChildElement("vsp_name");

    printf("user_id: %s\n", user_id->GetText());
    printf("user1_account: %s\n", user1_account->GetText());
    printf("user_name: %s\n", user_name->GetText());
    printf("big_visit_num: %s\n", big_visit_num->GetText());
    printf("vsp_id: %s\n", vsp_id->GetText());
    printf("vsp_name: %s\n", vsp_name->GetText());
    printf("================================================\n");
}
void XmlReader::xmlReadContactInfo(void)
{
    XMLDocument doc;
    doc.LoadFile(m_filename);
    if (doc.ErrorID() != 0) {
        printf("read xml error!\n");
        return;
    }
    XMLElement *root = doc.FirstChildElement("root");

    XMLElement *contacts = root->FirstChildElement("contacts");
    for (XMLElement *contact = contacts->FirstChildElement("contact");
         contact;
         contact = contact->NextSiblingElement("contact")) {
        XMLElement *user2_id = contact->FirstChildElement("user_id");
        XMLElement *user2_account = contact->FirstChildElement("user_acount");
        XMLElement *user2_name = contact->FirstChildElement("user_name");

        printf("user_id: %s\n", user2_id->GetText());
        printf("user_account: %s\n",  user2_account->GetText());
        printf("user_name: %s\n", user2_name->GetText());
        printf("\n");
    }

}
void XmlReader::xmlReadMeetingInfo(void)
{
    XMLDocument doc;
    doc.LoadFile(m_filename);
    if (doc.ErrorID() != 0) {
        printf("read xml error!\n");
        return;
    }
    XMLElement *root = doc.FirstChildElement("root");

    XMLElement *meetings = root->FirstChildElement("meetings");
    for (XMLElement *meeting = meetings->FirstChildElement("meeting");
         meeting;
         meeting = meeting->NextSiblingElement("meeting")) {
        XMLElement *meeting_id = meeting->FirstChildElement("meeting_id");
        XMLElement *meeting_name = meeting->FirstChildElement("meeting_name");
        XMLElement *start_time = meeting->FirstChildElement("start_time");
        XMLElement *end_time = meeting->FirstChildElement("end_time");
        XMLElement *meeting_conferrees = meeting->FirstChildElement("meeting_conferrees");

        printf("meeting_id: %s\n", meeting_id->GetText());
        printf("meeting_name: %s\n", meeting_name->GetText());
        printf("start_time: %s\n", start_time->GetText());
        printf("end_time: %s\n", end_time->GetText());
        printf("conferreess:\n");
        if (meeting_conferrees) {
            for (XMLElement *meeting_conferree = meeting_conferrees->FirstChildElement("meeting_conferree");
                 meeting_conferree;
                 meeting_conferree = meeting_conferree->NextSiblingElement("meeting_conferree")) {

                XMLElement *user2_id = meeting_conferree->FirstChildElement("user_id");
                XMLElement *user2_account = meeting_conferree->FirstChildElement("user_acount");
                XMLElement *user2_name = meeting_conferree->FirstChildElement("user_name");

                printf("user_id: %s\n", user2_id->GetText());
                printf("user_account: %s\n",  user2_account->GetText());
                printf("user_name: %s\n", user2_name->GetText());
            }
        }
        printf("\n");
    }
}
void XmlReader::xmlReadMsmServerInfo(void)
{
    XMLDocument doc;
    doc.LoadFile(m_filename);
    if (doc.ErrorID() != 0) {
        printf("read xml error!\n");
        return;
    }
    XMLElement *root = doc.FirstChildElement("root");

    XMLElement *msms = root->FirstChildElement("msms");
    for (XMLElement *msm = msms->FirstChildElement("msm");
         msm;
         msm = msm->NextSiblingElement("msm")) {
        XMLElement *msm_ip = msm->FirstChildElement("msm_ip");
        XMLElement *msm_port = msm->FirstChildElement("msm_port");

        printf("msm_ip: %s\n", msm_ip->GetText());
        printf("msm_port: %s\n", msm_port->GetText());
    }
}
void XmlReader::xmlReadAll(void)
{
    XMLDocument doc;
    doc.LoadFile(m_filename);
    if (doc.ErrorID() != 0) {
        printf("read xml error!\n");
        return;
    }

    XMLElement *root = doc.FirstChildElement("root");
    XMLElement *dev_id = root->FirstChildElement("dev_id");
    XMLElement *user_account = root->FirstChildElement("user_acount");
    XMLElement *vsp_url = root->FirstChildElement("vsp_url");
    XMLElement *code = root->FirstChildElement("code");
    XMLElement *msg = root->FirstChildElement("msg");

    printf("dev_id: %s\n", dev_id->GetText());
    printf("user_account: %s\n", user_account->GetText());
    printf("vsp_url: %s\n", vsp_url->GetText());;
    printf("code: %s\n", code->GetText());
    printf("msg: %s\n", msg->GetText());
    printf("============================================\n");

    XMLElement *user = root->FirstChildElement("user");
    XMLElement *user_id = user->FirstChildElement("user_id");
    XMLElement *user1_account = user->FirstChildElement("user_acount");
    XMLElement *user_name = user->FirstChildElement("user_name");
    XMLElement *big_visit_num = user->FirstChildElement("big_visit_num");
    XMLElement *vsp_id = user->FirstChildElement("vsp_id");
    XMLElement *vsp_name = user->FirstChildElement("vsp_name");

    printf("user_id: %s\n", user_id->GetText());
    printf("user1_account: %s\n", user1_account->GetText());
    printf("user_name: %s\n", user_name->GetText());
    printf("big_visit_num: %s\n", big_visit_num->GetText());
    printf("vsp_id: %s\n", vsp_id->GetText());
    printf("vsp_name: %s\n", vsp_name->GetText());
    printf("================================================\n");

    XMLElement *contacts = root->FirstChildElement("contacts");
    for (XMLElement *contact = contacts->FirstChildElement("contact");
         contact;
         contact = contact->NextSiblingElement("contact")) {
        XMLElement *user2_id = contact->FirstChildElement("user_id");
        XMLElement *user2_account = contact->FirstChildElement("user_acount");
        XMLElement *user2_name = contact->FirstChildElement("user_name");

        printf("user_id: %s\n", user2_id->GetText());
        printf("user_account: %s\n",  user2_account->GetText());
        printf("user_name: %s\n", user2_name->GetText());
        printf("\n");
    }

    printf("================================================\n");

    XMLElement *meetings = root->FirstChildElement("meetings");
    for (XMLElement *meeting = meetings->FirstChildElement("meeting");
         meeting;
         meeting = meeting->NextSiblingElement("meeting")) {
        XMLElement *meeting_id = meeting->FirstChildElement("meeting_id");
        XMLElement *meeting_name = meeting->FirstChildElement("meeting_name");
        XMLElement *start_time = meeting->FirstChildElement("start_time");
        XMLElement *end_time = meeting->FirstChildElement("end_time");
        XMLElement *meeting_conferrees = meeting->FirstChildElement("meeting_conferrees");

        printf("meeting_id: %s\n", meeting_id->GetText());
        printf("meeting_name: %s\n", meeting_name->GetText());
        printf("start_time: %s\n", start_time->GetText());
        printf("end_time: %s\n", end_time->GetText());
        printf("conferreess:\n");
        if (meeting_conferrees) {
            for (XMLElement *meeting_conferree = meeting_conferrees->FirstChildElement("meeting_conferree");
                 meeting_conferree;
                 meeting_conferree = meeting_conferree->NextSiblingElement("meeting_conferree")) {

                XMLElement *user2_id = meeting_conferree->FirstChildElement("user_id");
                XMLElement *user2_account = meeting_conferree->FirstChildElement("user_acount");
                XMLElement *user2_name = meeting_conferree->FirstChildElement("user_name");

                printf("user_id: %s\n", user2_id->GetText());
                printf("user_account: %s\n",  user2_account->GetText());
                printf("user_name: %s\n", user2_name->GetText());
            }
            }
        printf("\n");
    }

    printf("==========================================================\n");
    XMLElement *msms = root->FirstChildElement("msms");
    for (XMLElement *msm = msms->FirstChildElement("msm");
         msm;
         msm = msm->NextSiblingElement("msm")) {
        XMLElement *msm_ip = msm->FirstChildElement("msm_ip");
        XMLElement *msm_port = msm->FirstChildElement("msm_port");

        printf("msm_ip: %s\n", msm_ip->GetText());
        printf("msm_port: %s\n", msm_port->GetText());
    }

}
