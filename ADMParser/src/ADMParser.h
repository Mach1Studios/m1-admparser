#pragma once

#include <string>
#include <vector>
#include <math.h>
#include <iostream>

#include "pugixml.hpp"
#include "2094_common_definitions.xml.h"

#ifndef PI
#define PI 3.14159265358979323846f
#endif

using namespace std;

class ADMParser {

private:
	struct ADMBlock {
		std::string id;
		float x;
		float y;
		float z;
	};

	struct ADMAudioChannel {
		std::string id;
		std::vector<ADMBlock> blocks;
		bool valid;
	};

	struct ADMAudioObject {
		std::string id;
		std::vector<std::string> audioTracks;
		std::vector<ADMAudioChannel> audioChannels;
	};

	struct ADMAudioContent {
		std::string id;
		std::vector<ADMAudioObject> audioObjects;
	};

	struct ADMAudioProgramme {
		std::string id;
		std::vector<ADMAudioContent> audioContents;
	};

	struct ADMDocument {
		std::vector<ADMAudioProgramme> audioProgrammes;
	};

	static bool FindAudioChannel(pugi::xml_document& doc, string rootTag, string audioPackFormatIDRef, vector<ADMAudioChannel>& admAudioChannels) {
		bool bFound = false;

		for (pugi::xml_node xmlAudioPackFormat : doc.child(rootTag.c_str()).child("coreMetadata").child("format").child("audioFormatExtended").children("audioPackFormat"))
		{
			if (xmlAudioPackFormat.attribute("audioPackFormatID").as_string() == audioPackFormatIDRef)
			{
				for (pugi::xml_node xmlAudioChannelFormatIDRef : xmlAudioPackFormat.children("audioChannelFormatIDRef"))
				{
					string audioChannelFormatIDRef = xmlAudioChannelFormatIDRef.text().as_string();
					for (pugi::xml_node xmlAudioChannelFormat : doc.child(rootTag.c_str()).child("coreMetadata").child("format").child("audioFormatExtended").children("audioChannelFormat"))
					{
						if (xmlAudioChannelFormat.attribute("audioChannelFormatID").as_string() == audioChannelFormatIDRef)
						{
							ADMAudioChannel admAudioChannel;
							admAudioChannel.id = audioChannelFormatIDRef;

							int typeLabel = xmlAudioChannelFormat.attribute("typeLabel").as_int();
							if (typeLabel == 1 || typeLabel == 3)
							{
								admAudioChannel.valid = true;

								for (pugi::xml_node xmlAudioBlockFormat : xmlAudioChannelFormat.children("audioBlockFormat")) {

									ADMBlock admBlock;
									admBlock.id = xmlAudioBlockFormat.attribute("audioBlockFormatID").as_string();

									bool cartesian = xmlAudioBlockFormat.child("cartesian").text().as_int();
									if (cartesian) {
										for (pugi::xml_node position : xmlAudioBlockFormat.children("position"))
										{
											std::string coordinate = position.attribute("coordinate").as_string();
											float val = position.text().as_float();
											if (coordinate == "X") {
												admBlock.x = val;
											}
											else if (coordinate == "Y") {
												admBlock.y = val;
											}
											else if (coordinate == "Z") {
												admBlock.z = val;
											}
											//std::cout << "position: " << coordinate << " = " << position.text().as_float() << "\n";
										}
									}
									else {
										float azimuth = 0;
										float elevation = 0;
										float distance = 0;
										for (pugi::xml_node position : xmlAudioBlockFormat.children("position"))
										{
											std::string coordinate = position.attribute("coordinate").as_string();
											float val = position.text().as_float();
											if (coordinate == "azimuth") {
												azimuth = val;
											}
											else if (coordinate == "elevation") {
												elevation = val;
											}
											else if (coordinate == "distance") {
												distance = val;
											}
											//std::cout << "position: " << coordinate << " = " << position.text().as_float() << "\n";
										}
										admBlock.x = distance * cos(elevation * PI / 180) * cos(azimuth * PI / 180);
										admBlock.y = distance * cos(elevation * PI / 180) * sin(azimuth * PI / 180);
										admBlock.z = distance * sin(elevation * PI / 180);
									}
									admAudioChannel.blocks.push_back(admBlock);
								}
							}
							else
							{
								admAudioChannel.valid = false;
							}
							admAudioChannels.push_back(admAudioChannel);
							bFound = true;
						}
					}
				}
			}
		}
		return bFound;
	}


	ADMDocument admDocument;

public:

	struct Point {
		std::string audioTrack;
		float x;
		float y;
		float z;
	};

    bool sessionDataFound = false;
    
    bool findSessionDetails(pugi::xml_document& doc){
        pugi::xml_node xmlAudioSessionTitle = doc.child("ebuCoreMain").child("coreMetadata").child("title");
        std::string dctitle = xmlAudioSessionTitle.child("dc:title").text().as_string();
        std::cout << "Title: " << dctitle << "\n";
        
        pugi::xml_node xmlAudioSessionCreator = doc.child("ebuCoreMain").child("coreMetadata").child("creator");
        std::string organizationName = xmlAudioSessionCreator.child("organisationDetails").child("organisationName").text().as_string();
        std::cout << "Creator: " << organizationName << "\n";
        
        pugi::xml_node xmlAudioSessionDescription = doc.child("ebuCoreMain").child("coreMetadata").child("description");
        std::string dcdesc = xmlAudioSessionDescription.child("dc:description").text().as_string();
        std::cout << "Description: " << dcdesc << "\n";
        
        pugi::xml_node xmlAudioSessionDate = doc.child("ebuCoreMain").child("coreMetadata").child("date").child("created");
        std::string createdDate = xmlAudioSessionDate.attribute("startDate").as_string();
        std::cout << "Date: " << createdDate << "\n";
        sessionDataFound = true;

		return sessionDataFound;
    }
    
	bool ParseString(const char* string, vector<Point>& points) {
		pugi::xml_document doc;

		if (doc.load_string(string)) {
			return ParseXMLDoc(doc, points);
		}

		return false;
	}

	bool ParseFile(const char* filename, vector<Point>& points) {
		pugi::xml_document doc;

		if (doc.load_file(filename)) {
			return ParseXMLDoc(doc, points);
		}

		return false;
	}

	bool ParseXMLDoc(pugi::xml_document& doc, vector<Point>& points) {

		pugi::xml_document docCommonDefinitions;

		if (docCommonDefinitions.load_buffer(COMMON_DEFINITIONS_XML, COMMON_DEFINITIONS_XML_SIZE, pugi::encoding_auto)) {
			admDocument = ADMDocument();
            
            if (!sessionDataFound){
                findSessionDetails(doc);
            }
            
			for (pugi::xml_node xmlAudioProgramme : doc.child("ebuCoreMain").child("coreMetadata").child("format").child("audioFormatExtended").children("audioProgramme"))
			{
				ADMAudioProgramme admAudioProgramme;
				admAudioProgramme.id = xmlAudioProgramme.attribute("audioProgrammeID").as_string();

				for (pugi::xml_node xmlAudioContentIDRef : xmlAudioProgramme.children("audioContentIDRef"))
				{
					string audioContentIDRefID = xmlAudioContentIDRef.text().as_string();
					for (pugi::xml_node xmlAudioContent : doc.child("ebuCoreMain").child("coreMetadata").child("format").child("audioFormatExtended").children("audioContent"))
					{
						if (xmlAudioContent.attribute("audioContentID").as_string() == audioContentIDRefID)
						{
							ADMAudioContent admAudioContent;
							admAudioContent.id = audioContentIDRefID;

							for (pugi::xml_node xmlAudioObjectIDRef : xmlAudioContent.children("audioObjectIDRef"))
							{
								string xmlAudioObjectIDRefValue = xmlAudioObjectIDRef.text().as_string();
								for (pugi::xml_node xmlAudioObject : doc.child("ebuCoreMain").child("coreMetadata").child("format").child("audioFormatExtended").children("audioObject"))
								{
									if (xmlAudioObject.attribute("audioObjectID").as_string() == xmlAudioObjectIDRefValue)
									{
										ADMAudioObject admAudioObject;
										admAudioObject.id = xmlAudioObjectIDRefValue;
										
										for (pugi::xml_node xmlAudioTrackUIDRef : xmlAudioObject.children("audioTrackUIDRef"))
										{
											admAudioObject.audioTracks.push_back(xmlAudioTrackUIDRef.text().as_string());
										}

										string audioPackFormatIDRef = xmlAudioObject.child("audioPackFormatIDRef").text().as_string();

										// find audio channel
										if (!FindAudioChannel(doc, "ebuCoreMain", audioPackFormatIDRef, admAudioObject.audioChannels)) {
											FindAudioChannel(docCommonDefinitions, "ituADM", audioPackFormatIDRef, admAudioObject.audioChannels);
										}
										admAudioContent.audioObjects.push_back(admAudioObject);
									}
								}
							}
							admAudioProgramme.audioContents.push_back(admAudioContent);
						}
					}
				}
				admDocument.audioProgrammes.push_back(admAudioProgramme);
			}

			points.clear();
			for (auto audioProgramme : admDocument.audioProgrammes) {
				for (auto audioContent : audioProgramme.audioContents) {
					for (auto audioObject : audioContent.audioObjects) {

						if (audioObject.audioChannels.size() == audioObject.audioTracks.size()) { // skip object with not equal channels and tracks count
							for (int i = 0; i < audioObject.audioChannels.size(); i++) {
								for (int j = 0; j < audioObject.audioChannels[i].blocks.size(); j++) {
									ADMParser::ADMBlock block = audioObject.audioChannels[i].blocks[j];

									Point point;
									point.audioTrack = audioObject.audioTracks[i];
									point.x = block.x;
									point.y = block.y;
									point.z = block.z;
									points.push_back(point);
								}
							}
						}

					}
				}
			}
			return true;
		}

		return false;
	}
};
