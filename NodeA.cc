#include <omnetpp.h>
#include <algorithm>
#include <random>
#include <fstream>


using namespace omnetpp;

class NodeA : public cSimpleModule {
  private:
    double qTable[3][3]; // States x Actions
    double learningRate;
    double discountFactor;
    double explorationRate;

    int currentState;
    int currentAction;

  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    int chooseAction();
    void updateQTable(int nextState, bool successfulTransmission, double energyConsumed);
    std::vector<double> totalRewards;
    virtual void finish() override;

};

Define_Module(NodeA);

void NodeA::initialize() {
    // Initialize Q-table with zeros
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            qTable[i][j] = 0;
        }
    }

    // Initialize learning parameters
    learningRate = 0.5;
    discountFactor = 0.9;
    explorationRate = 0.1;

    // Select initial state and action
    currentState = 0;
    currentAction = chooseAction();

    // Schedule the first packet transmission
    scheduleAt(simTime() + 1, new cMessage("SendPacket"));
}

int NodeA::chooseAction() {
    if (uniform(0, 1) < explorationRate) {
        // Exploration: Choose a random action
        return intrand(3);
    } else {
        // Exploitation: Choose the action with the highest Q-value for the current state
        return std::max_element(qTable[currentState], qTable[currentState] + 3) - qTable[currentState];
    }
}

void NodeA::updateQTable(int nextState, bool successfulTransmission, double energyConsumed) {
    double reward = (successfulTransmission ? 10 : -10) - energyConsumed;
    double maxQ = *std::max_element(qTable[nextState], qTable[nextState] + 3);
    qTable[currentState][currentAction] +=
        learningRate * (reward + discountFactor * maxQ - qTable[currentState][currentAction]);
}


void NodeA::handleMessage(cMessage *msg) {
    if (strcmp(msg->getName(), "SendPacket") == 0) {
        // Send packet to Node B with the current transmission power
        cMessage *packet = new cMessage("Packet");
        packet->addPar("transmissionPower") = currentState;
        send(packet, "out");

        // Schedule the next packet transmission
        scheduleAt(simTime() + 1, msg);
    } else {
        // Receive feedback from Node B (success or failure of packet transmission)
        bool successfulTransmission = msg->par("successfulTransmission");
        double energyConsumed = msg->par("energyConsumed");

        // Update the Q-table
        int nextState = (currentState + currentAction) % 3;
        updateQTable(nextState, successfulTransmission, energyConsumed);

        // Choose a new action based on the received feedback
        currentState = nextState;
        currentAction = chooseAction();

        // Calculate and store the total reward
        double reward = (successfulTransmission ? 10 : -10) - energyConsumed;
        totalRewards.push_back(reward);

        delete msg;
    }
}

void NodeA::finish() {
    std::ofstream outFile;
    outFile.open("total_rewards.txt");
    for (const auto &reward : totalRewards) {
        outFile << reward << std::endl;
    }
    outFile.close();
}
