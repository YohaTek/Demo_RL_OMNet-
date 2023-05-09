#include <omnetpp.h>

using namespace omnetpp;

class NodeB : public cSimpleModule {
  protected:
    virtual void handleMessage(cMessage *msg) override;
};

Define_Module(NodeB);

void NodeB::handleMessage(cMessage *msg) {
    if (strcmp(msg->getName(), "Packet") == 0) {
        // Calculate the success of packet transmission based on the transmission power
        int transmissionPower = msg->par("transmissionPower");
        double packetLossProbability;
        double energyConsumed;

        if (transmissionPower == 0) { // Low power
            packetLossProbability = 0.3;
            energyConsumed = 1;
        } else if (transmissionPower == 1) { // Medium power
            packetLossProbability = 0.1;
            energyConsumed = 2;
        } else { // High power
            packetLossProbability = 0.05;
            energyConsumed = 4;
        }

        bool successfulTransmission = uniform(0, 1) > packetLossProbability;

        // Send feedback to Node A
        cMessage *feedback = new cMessage("Feedback");
        feedback->addPar("successfulTransmission") = successfulTransmission;
        feedback->addPar("energyConsumed") = energyConsumed;
        send(feedback, "out");
    }

    delete msg;
}

