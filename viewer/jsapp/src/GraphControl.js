import React from 'react';
import { XYGraph } from './XYGraph';
import { StatsGraph } from './StatsGraph';

export class GraphControl extends React.Component {
    render() {
        const graphType = this.props.graph ? this.props.graph.type : undefined;

        switch (this.props.graph.type) {
            case "XY":
                return (<XYGraph title={this.props.graph.id}
                                properties={this.props.graph.properties} 
                                data={this.props.graph.data} 
                                bgColor="#FF77AA" />);
            case "STATS":
                return <StatsGraph data={this.props.graph.data} />
        default:
            return <div>Unknown graph type {this.props.graph.type}</div>;
        }
    }
}