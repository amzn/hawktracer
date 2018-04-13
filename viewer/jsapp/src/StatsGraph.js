import React from 'react';

export class StatsGraph extends React.Component {
    render() {
        const rangeEventCount = this.props.data ? this.props.data.rangeEventCount : 'NaN';
        return (
            <div>
                <h3>Number of events: {rangeEventCount}</h3>
            </div>)
    }
}

StatsGraph.defaultProps = {
    data: undefined
};