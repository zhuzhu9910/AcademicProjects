#ifndef MESSAGE_H
#define MESSAGE_H
/////////////////////////////////////////////////////////////////////
// Message.h   -  Defines EndPoints and Messages                   //
// ver 1.0                                                         //
// Language:      Visual C++, 2008                                 //
// Platform:      Dell Dimension T7400, Win 7 Pro                  //
// Application:   Utility for CSE687 projects                      //
// Author:        Jim Fawcett, Syracuse University, CST 4-187      //
//                (315) 443-3948, jfawcett@twcny.rr.com            //
/////////////////////////////////////////////////////////////////////

#include <string>

/////////////////////////////////////////////////////////////////////////
// Manage addresses in the form 127.0.0.1:8000

class EndPoint //: public IEndPoint
{
public:
  EndPoint(const std::string& address, long port);
  EndPoint(const std::string& eps);
  std::string address();
  long port();
  std::string& ToString();
private:
  std::string ep;
};

/////////////////////////////////////////////////////////////////////////
// Create, serialize, and deserialize Message instances

class Message //: public IMessage
{
public:
  enum MsgType { text, file };
  Message(const std::string& text = "");
  Message(const std::string& body, EndPoint ep, MsgType type);
  Message(const Message& msg);
  std::string& getEndPoint();
  MsgType&  getMsgType();
  std::string& body();
  EndPoint& returnAddress();
  std::string TypeToString();
  std::string ToString();
  static Message FromString(const std::string& xml);
private:
  static std::string trimQuotes(const std::string& s);
  EndPoint returnAddr_;
  std::string body_;
  MsgType type_;
};

#endif
