import React, { Component } from 'react';
import { KlassFieldSelector } from './KlassFieldSelector';
import { GraphControl } from './GraphControl';
import { ConnectionIndicator } from './ConnectionIndicator';
import { Connection } from './Connection';
import { TimeRangeSelector } from './TimeRangeSelector'
import { Protocol } from './Protocol'
import { GraphTypeSelector } from './GraphTypeSelector'

function generateGraphID(klassID, fieldName) {
    return klassID + "#" + fieldName;
}

class App extends Component {
    connection = new Connection();
    protocol = new Protocol();
    
    selectedKlass = undefined;
    selectedField = undefined;

    constructor(props) {
        super(props);

        this.state = {
            graphs: [],
            klasses: [],
            graphTypes: [],
            connectionStatus: 'notconnected',
            totalTSRange: {first: 0, last: 0}
        }

        this.initProtocol();

        this.changeKlassField = this.changeKlassField.bind(this);
        this.connectionStateChangeRequest = this.connectionStateChangeRequest.bind(this);
        this.changeTimeRange = this.changeTimeRange.bind(this);
        this.makeGraphRequest = this.makeGraphRequest.bind(this);

        this.connection.onConnected = function() {
            this.setState({connectionStatus: 'connected', graphs: [], klasses: []});
            this.connection.send(this.protocol.makeNewGraphRequest(
                "Statistics", "STATS", {}));

            setInterval(function() {
                this.connection.send(this.protocol.makeGetTotalTSRangeRequest());
                this.connection.send(this.protocol.makeGetDataRequest());
            }.bind(this), 100);
        }.bind(this);
        this.connection.onDisconnected = function() {
            this.setState({connectionStatus: 'notconnected'});
        }.bind(this);
        this.connection.onMessage = this.protocol.handleMessage.bind(this.protocol);
    }

    initProtocol() {
        this.protocol
            .registerAddKlassHandler(this.addKlass.bind(this))
            .registerAddFieldHandler(this.addField.bind(this))
            .registerDataHandler(this.updateGraph.bind(this))
            .registerUpdateTSRangeHandler(this.updateTSRange.bind(this))
            .registerGraphTypesHandler(this.updateGraphTypes.bind(this))
            .registerErrorHandler((object) => console.error(object.message))
            .registerSetGraphPropertiesHandler(this.setGraphProperties.bind(this));
    }

    addKlass(obj) {
        if (this.state.klasses.find(element => element.name == obj.name) != undefined) {
            return;
        }

        let newKlass = {
            name: obj.name,
            id: obj.id,
            fields: []
        };
        let tmpKlasses = this.state.klasses;
        tmpKlasses.push(newKlass);
        this.setState({klasses: tmpKlasses});
        if (this.selectedKlass == undefined) {
            this.changeKlassField(newKlass, undefined);
        }
    }

    addField(obj) {
        let tmpKlasses = this.state.klasses;
        let klass = tmpKlasses.find(element => element.id == obj.klassId);
        if (klass == undefined) {
            return;
        }

        if (klass.fields.find(element => element.name == obj.name) != undefined) {
            return;
        }

        let newField = {name: obj.name};
        klass.fields.push(newField);
        this.setState({klasses: tmpKlasses});

        if (klass == this.selectedKlass && this.selectedField == undefined) {
            this.changeKlassField(klass, newField);
        }
    }

    updateGraph(obj) {
        let graphs = this.state.graphs;

        let graph = graphs.find(g => g.id == obj.graphId);
        if (graph == undefined) {
            return;
        }

        graph.data = obj.data;
        this.setState({graphs: graphs});
    }

    updateGraphTypes(obj) {
        this.setState({graphTypes: obj.graphs});
    }

    updateTSRange(obj) {
        this.setState({totalTSRange: {first: obj.startTimestamp, last: obj.stopTimestamp}});
    }

    changeKlassField(klass, field) {
        this.selectedKlass = klass;
        this.selectedField = field;
    }

    changeTimeRange(duration, stopTimestamp) {
        this.connection.send(this.protocol.makeSetCurrentTSRangeRequest(duration, stopTimestamp));
    }

    connectionStateChangeRequest(uri) {
        if (this.connection.state == this.connection.State.connected) {
            this.connection.disconnect();
        } else {
            this.connection.connect(uri);
        }
    }

    makeGraphRequest(graph) {
        this.connection.send(this.protocol.makeNewGraphRequest(
            graph.name,
            graph.selectedGraphType.id, 
            {klassId: graph.klass.id, fieldMap: graph.fieldMap}));
    }

    setGraphProperties(obj) {
        let graphs = this.state.graphs;
        let graph = graphs.find((g) => g.id == obj.graphId);

        if (graph) {
            graph.properties = obj.properties;
        } else {
            graphs = graphs.concat([{
                id: obj.graphId,
                type: obj.graphTypeId,
                data: undefined,
                properties: obj.properties
            }]);
        }

        this.setState({
            graphs: graphs
        });
    }

    render() {
        return (
            <div>
                <ConnectionIndicator status={this.state.connectionStatus} onStateChangeRequest={this.connectionStateChangeRequest} />
                <TimeRangeSelector onRangeChanged={this.changeTimeRange} totalRange={this.state.totalTSRange} />
                <GraphTypeSelector onGraphRequested={this.makeGraphRequest} klasses={this.state.klasses} graphTypes={this.state.graphTypes} />
                <div>
                    {this.state.graphs.map((graph) => { return (
                        <div>
                            <GraphControl graph={graph} />
                        </div>)})}
                </div>
            </div>)
    }
    
    wheel(e) { 
    }
}

export default App;
