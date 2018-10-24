#include "QtImage.h"
#include "Core/CommonDefs.h"

QtImage::QtImage()
{

}

QtImage::~QtImage()
{

}

bool QtImage::loadFromFile(const std::string &fileName)
{
    return img_.load(U2Q(fileName));
}

bool QtImage::loadFromRawData(DataFormat dt, int width, int height, uint8_t* data, size_t dataSize, void* parameter)
{
    return true;
}

QImage QtImage::toQImage()
{
    return img_;
}

bool QtImage::isNull() const
{
    return img_.isNull();
}

bool QtImage::saveToFile(const std::string &fileName) const
{
    return img_.save(U2Q(fileName));
}

