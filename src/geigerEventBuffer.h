class GeigerEventBuffer
{
    unsigned long * geigerDatastore;
    
  public:
    GeigerEventBuffer();
    void addTimestamp(unsigned long);
    bool hasTimestamp();
    unsigned long getTimestamp();  
};