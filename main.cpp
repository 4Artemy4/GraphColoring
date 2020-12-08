#include <iostream>
#include "cpp/sc_addr.hpp"
#include "cpp/sc_memory.hpp"
#include "cpp/sc_iterator.hpp"

void runTest(const std::unique_ptr<ScMemoryContext> &context, std::string testNumber);

void printGraph(const std::unique_ptr<ScMemoryContext> &context, ScAddr graph, ScAddr rrel_arcs, ScAddr rrel_nodes);

ScAddr fillGraph(const std::unique_ptr<ScMemoryContext> &context, ScAddr graph, int startColor);

bool isVertexInSet(const std::unique_ptr<ScMemoryContext> &context, ScAddr element, ScAddr set);
bool isVertexColored(const std::unique_ptr<ScMemoryContext> &context, ScAddr element, ScAddr set);
bool isAllVertexesColored(const std::unique_ptr<ScMemoryContext> &context, ScAddr graph,  ScAddr coloredVertexes);
void printColoredGraph(const std::unique_ptr<ScMemoryContext> &context, ScAddr groups);
bool isNeighbourHasSameColor(const std::unique_ptr<ScMemoryContext> &context, ScAddr graph,  ScAddr node, ScAddr color);
void delColorVertexes(const std::unique_ptr<ScMemoryContext> &context, ScAddr colors);
using namespace std;

int main() {
    sc_memory_params params;

    sc_memory_params_clear(&params);
    params.repo_path = "/home/ostis-master/ostis-web-platform-0.5.0/kb.bin";
    params.config_file = "/home/ostis-master/ostis-web-platform-0.5.0/config/sc-web.ini";
    params.ext_path = "/home/ostis-master/ostis-web-platform-0.5.0/sc-machine/bin/extensions";
    params.clear = SC_FALSE;

    ScMemory memory;
    memory.Initialize(params);
    const std::unique_ptr<ScMemoryContext> context(new ScMemoryContext(sc_access_lvl_make_max, "Graph Colors"));

    runTest(context, "1");
    runTest(context, "2");
    runTest(context, "3");
    runTest(context, "4");
    runTest(context, "5");
    return 0;
}

void runTest(const std::unique_ptr<ScMemoryContext> &context, std::string testNumber) {
    ScAddr graph = context->HelperResolveSystemIdtf("G" + testNumber);
    std::cout << "Graph" << testNumber << ":\n";
//    printGraph(context, graph, rrel_arcs, rrel_nodes);

    ScAddr result = fillGraph(context, graph, 1);
    printColoredGraph(context, result);
delColorVertexes(context, result);

}

ScAddr fillGraph(const std::unique_ptr<ScMemoryContext> &context, ScAddr graph, int startColor) {
    ScAddr colors = context->CreateNode(ScType::Const);
    ScAddr coloredVertexes = context->CreateNode(ScType::Const);

    while (!isAllVertexesColored(context, graph, coloredVertexes)) {
        ScAddr currentColor = context->CreateNode(ScType::Const);
        context->HelperSetSystemIdtf(to_string(startColor++), currentColor);
        context->CreateEdge(ScType::EdgeAccessConstPosPerm, colors, currentColor);

        ScIterator5Ptr nodesSet = context->Iterator5(graph, ScType::EdgeAccessConstPosPerm, ScType(0),
                                                     ScType::EdgeAccessConstPosPerm,
                                                     context->HelperResolveSystemIdtf("rrel_nodes"));

        nodesSet->Next();
        ScIterator3Ptr nodes = context->Iterator3(nodesSet->Get(2), ScType::EdgeAccessConstPosPerm, ScType(0));
        while (nodes->Next()) {
//        std::cout << context->HelperGetSystemIdtf(nodes->Get(2)) << '\n';

            if (!isVertexInSet(context, nodes->Get(2), coloredVertexes)) {
                if (!isNeighbourHasSameColor(context, graph, nodes->Get(2), currentColor)) {
//                    std::cout << context->HelperGetSystemIdtf(nodes->Get(2)) << " - colored\n";
                    context->CreateEdge(ScType::EdgeAccessConstPosPerm, currentColor, nodes->Get(2));
                    context->CreateEdge(ScType::EdgeAccessConstPosPerm, coloredVertexes, nodes->Get(2));
                }
            }
        }
    }
    context->EraseElement(coloredVertexes);
    return colors;
}

bool isNeighbourHasSameColor(const std::unique_ptr<ScMemoryContext> &context, ScAddr graph,  ScAddr node, ScAddr color){
    ScIterator5Ptr arcsSet = context->Iterator5(graph, ScType::EdgeAccessConstPosPerm, ScType(0),
                                                ScType::EdgeAccessConstPosPerm,
                                                context->HelperResolveSystemIdtf("rrel_arcs"));

    arcsSet->Next();
    ScIterator5Ptr neighbours = context->Iterator5(node, ScType::EdgeDCommon, ScType(0),
                                                   ScType::EdgeAccessConstPosPerm, arcsSet->Get(2));
    while (neighbours->Next()) {
        if (isVertexInSet(context, neighbours->Get(2), color)) {
            return true;
        }
    }
    return false;
}

bool isAllVertexesColored(const std::unique_ptr<ScMemoryContext> &context, ScAddr graph,  ScAddr coloredVertexes){
    ScIterator5Ptr nodesSet = context->Iterator5(graph, ScType::EdgeAccessConstPosPerm, ScType(0),
                                                 ScType::EdgeAccessConstPosPerm,
                                                 context->HelperResolveSystemIdtf("rrel_nodes"));

    nodesSet->Next();
    ScIterator3Ptr nodes = context->Iterator3(nodesSet->Get(2), ScType::EdgeAccessConstPosPerm, ScType(0));
    while (nodes->Next()){
        if (!isVertexInSet(context, nodes->Get(2), coloredVertexes)) {
//            std::cout << context->HelperGetSystemIdtf(nodes->Get(2)) << " - not in the set\n";
            return false;
        }
    }
    return true;
}

bool isVertexInSet(const std::unique_ptr<ScMemoryContext> &context, ScAddr element, ScAddr set) {
    ScIterator3Ptr location = context->Iterator3(set, ScType::EdgeAccessConstPosPerm, ScType(0));
    while (location->Next()) {
        if (location->Get(2) == element) {
            return true;
        }
    }
    return false;
}

void delColorVertexes(const std::unique_ptr<ScMemoryContext> &context, ScAddr colors){
    ScIterator3Ptr nodes = context->Iterator3(colors, ScType::EdgeAccessConstPosPerm, ScType(0));
    while (nodes->Next()){
        context->EraseElement(nodes->Get(2));
    }
    context->EraseElement(colors);
}


bool isVertexColored(const std::unique_ptr<ScMemoryContext> &context, ScAddr element, ScAddr set) {
    ScIterator3Ptr location = context->Iterator3(set, ScType::EdgeAccessConstPosPerm, ScType(0));
    while (location->Next()) {
        if (isVertexInSet(context, element, location->Get(2))) {
            return true;
        }
    }
    return false;
}

void printColoredGraph(const std::unique_ptr<ScMemoryContext> &context, ScAddr groups){
    ScIterator3Ptr colors = context->Iterator3(groups, ScType::EdgeAccessConstPosPerm, ScType(0));
    while (colors->Next()){
        ScIterator3Ptr vertexes = context->Iterator3(colors->Get(2), ScType::EdgeAccessConstPosPerm, ScType(0));
        std::cout << "group" << context->HelperGetSystemIdtf(colors->Get(2)) << ": ";
        while (vertexes->Next()){
            std::cout << context->HelperGetSystemIdtf(vertexes->Get(2)) << ", ";
        }
        std::cout << "\n";
    }
    std::cout << "\n\n";
}

void printGraph(const std::unique_ptr<ScMemoryContext> &context, ScAddr graph, ScAddr rrel_arcs, ScAddr rrel_nodes) {
    ScAddr v1, v2, printed_vertex;
    printed_vertex = context->CreateNode(ScType::Const);
    std::cout << context->HelperGetSystemIdtf(graph) << "\n----------------------\n";
    ScIterator5Ptr it = context->Iterator5(graph, ScType::EdgeAccessConstPosPerm, ScType(0),
                                           ScType::EdgeAccessConstPosPerm, rrel_arcs);
    it->Next();
    ScIterator3Ptr arcs_it = context->Iterator3(it->Get(2), ScType::EdgeAccessConstPosPerm, ScType(0));
    while (arcs_it->Next()) {
        v1 = context->GetEdgeSource(arcs_it->Get(2));
        v2 = context->GetEdgeTarget(arcs_it->Get(2));
        std::cout << context->HelperGetSystemIdtf(v1) << " -- " << context->HelperGetSystemIdtf(v2) << std::endl;
        if (!isVertexInSet(context, v1, printed_vertex))
            context->CreateEdge(ScType::EdgeAccessConstPosPerm, printed_vertex, v1);
        if (!isVertexInSet(context, v2, printed_vertex))
            context->CreateEdge(ScType::EdgeAccessConstPosPerm, printed_vertex, v2);
    }
}