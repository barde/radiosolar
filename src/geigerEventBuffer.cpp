#include <vector>

using std::vector;

class GeigerEventBuffer
{
    vector<unsigned long> geigerDatastore;

    public:
        GeigerEventBuffer();
        void addTimestamp(unsigned long);
        bool hasTimestamp();
        unsigned long getTimestamp();
};

GeigerEventBuffer::GeigerEventBuffer()
{
}

void GeigerEventBuffer::addTimestamp(unsigned long geigerTimestamp)
{
    this->geigerDatastore.push_back(geigerTimestamp);
}

bool GeigerEventBuffer::hasTimestamp()
{
    return this->geigerDatastore.size() != 0;
}

unsigned long GeigerEventBuffer::getTimestamp()
{
    if(!this->hasTimestamp())
    {
        return 0;
    }

    auto oldestTimestamp = this->geigerDatastore[0];
    this->geigerDatastore.erase(geigerDatastore.begin());

    return oldestTimestamp;
}