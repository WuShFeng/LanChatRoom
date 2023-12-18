//
// Created by WuShF on 2023/12/18.
//

#include "FileConvert.h"
#include"QBuffer"

QString FileConvert::imageToBase64(QImage &image) {
    QByteArray byteArray;
    QBuffer buffer(&byteArray);
    buffer.open(QIODevice::WriteOnly);
    image.save(&buffer, "PNG");
    return QString("data:image/png;base64,") + byteArray.toBase64();
}
