namespace cpp testFunctions

service simpleTests
{
   void ping(),
   bool sendToServer(1:string a),
   string sendToClient()
}