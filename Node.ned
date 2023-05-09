simple NodeA
{
    parameters:
        double packetGenerationInterval @unit(s);
        int numPowerLevels;
    gates:
        input in;
        output out;
}
simple NodeB
{
    parameters:
        double packetGenerationInterval @unit(s);
        int numPowerLevels;
    gates:
        input in;
        output out;
}
network SimpleNetwork
{
    @display("bgb=263,228");
    submodules:
        nodeA: NodeA {
            @display("p=32,85");
        }
        nodeB: NodeB {
            @display("p=194,85");
        }
    connections:
        nodeA.out --> nodeB.in;
        nodeB.out -->nodeA.in;
}
