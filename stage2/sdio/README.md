# 第一个例子 `nand_1`

#Card Identification Mode
While in card identification mode the host resets all the cards that are in card identifcation mode, validates operation voltage range, 


## Two Status Information of SD Memory Card

The SD Memory Card supports two status fields as follows:

- 'Card Status': Error and state information of a executed command, indicated in the response.

- 'SD Status': Extended status field of 512 bits that supports special features of the SD Memory Card and future Application-Specific features.

### Card Status

The response format R1 contains a 32-bit field named card status. This field is intended to transmit the card's status information (which may be stored in a local status register) to the host. If not specified otherwise, the status entries are always related to the previous issued command.

The type and clear condition fields in the table are 

#SDIO
## General Description

The SDIO(Secure Digital I/O) card is based on and compatible with the SD memory card. This compatibility includes mechanical, electrical, power, signaling and software. The intent of the SDIO card is to provide high speed data I/O with low power consumption for mobile electronic devices. A primary goal is that an SDIO inserted into a non-SDIO aware host will cause no physical damage or disruption of that device or it's software. In this case, the SDIO card should simply be ignored. Once inserted into an SDIO aware host, the detection of the card will be via normal means described in the SD specification with some extensions. In this state, the SDIO card will be idle and draw a small amount of power (15 mA averaged over 1 second). During the normal initialization and interrogation of the card by the host, the card will identify itself as an SDIO device. The host software will then obtain the card information in a tuple format and determine if that card's I/O function(s) are acceptable to activate. This decision will be based on such parameters as power requirements or the availability of appropriate software drivers. If the card is acceptable, it will be allowed to power up fully and start the I/O functions built into it.

#### SDIO feature
1. Target
et 
