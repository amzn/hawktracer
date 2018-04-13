import React from 'react'
import { KlassFieldSelector } from './KlassFieldSelector'

export class GraphTypeSelector extends React.Component {
    constructor(props) {
        super(props);

        this.graphTypeChange = this.graphTypeChange.bind(this);
        this.onKlassFieldChanged = this.onKlassFieldChanged.bind(this);
        this.addGraph = this.addGraph.bind(this);
        this.handleGraphNameChange = this.handleGraphNameChange.bind(this);

        this.state = {
            name: 'Graph',
            selectedGraphType: undefined,
            fieldMap: {}
        };
    }

    componentWillReceiveProps(nextProps) {
        /*if (nextProps.klasses.length > 0 && nextProps.klasses[0].fields.length > 0) {
            let initFieldMap = {};
            initFieldMap[this.props.graphTypes[0].name] = {
                klass: nextProps.klasses[0],
                field: nextProps.klasses[0].fields[0]
            };
            this.setState({fieldMap: initFieldMap});
        }*/
    }

    graphTypeChange(e) {
        var found = this.props.graphTypes.find(function(element) {
            return element.id == e.target.value;
        });

        if (found) {
            this.setState({selectedGraphType: found, fieldMap: {}});
        }
    }

    addGraph() {
        this.props.onGraphRequested(this.state);
    }

    onKlassFieldChanged(klass, field, graphFieldName) {
        let fieldMap = this.state.fieldMap;
        fieldMap[graphFieldName] = field.name;
        this.setState({fieldMap: fieldMap, klass: klass});
    }

    handleGraphNameChange(evt) {
        this.setState({name: evt.target.value});
    }

    render() {
        return (
            <div>
                Name: <input type="text" value={this.state.name} onChange={this.handleGraphNameChange} />
                <select onChange={this.graphTypeChange}>
                    {this.props.graphTypes.map((value) => <option value={value.id}>{value.id}</option>)}
                </select>
                <GraphFields klasses={this.props.klasses} onKlassFieldChanged={this.onKlassFieldChanged} graph={this.state.selectedGraphType} />
                <button onClick={this.addGraph}>Add</button>
            </div>)
    }
}

class GraphFields extends React.Component {
    render () {
        if (this.props.graph) {
            return (<span>{this.props.graph.fields.map((value) => <div>{value}: <KlassFieldSelector name={value} klasses={this.props.klasses} onKlassFieldChanged={this.props.onKlassFieldChanged} /></div>)}</span>)
        } else {
            return <span></span>;
        }
    }
}