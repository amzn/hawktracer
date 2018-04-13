import React from 'react'
import PropTypes from 'prop-types';

export class TimeRangeSelector extends React.Component {
    defaultDuration = 10000;
    defaultStopTimestamp = -1;
    durationCtrlName = 'duration';
    stopTimestampCtrlName = 'stopTimestamp'

    constructor(props) {
        super(props);

        this.state = {
            duration: this.defaultDuration,
            stopTimestamp: this.defaultStopTimestamp
        }
        this.handleChange = this.handleChange.bind(this);
        this.changeRange = this.changeRange.bind(this);
        this.changeRangeToMinMax = this.changeRangeToMinMax.bind(this);
    }

    _formatTime(value, rounding) {
        var units = ['ns', 'us', 'ms', 's'];
        var unit_it = 0;
        
        while (value > 1000 && unit_it < units.length-1) {
            value /= 1000;
            unit_it++;
        }

        Math.round(value * rounding) / rounding;
        return {value: value, unit: units[unit_it]};
    }

    changeRange() {
        this.props.onRangeChanged(this.state.duration, this.state.stopTimestamp);
    }

    handleChange(evt) {
        if (evt.target.name == this.durationCtrlName) {
            this.setState({duration: parseInt(evt.target.value)});
        } else if (evt.target.name == this.stopTimestampCtrlName) {
            this.setState({stopTimestamp: parseInt(evt.target.value)});
        }
    }

    changeRangeToMinMax() {
        this.setState({
            duration: this.props.totalRange.last - this.props.totalRange.first,
            stopTimestamp: this.props.totalRange.last
        }, () => {
            this.changeRange();
        });
    }

    render() {
        const totalDuration = this._formatTime(this.props.totalRange.last - this.props.totalRange.first, 2);

        return (
            <div>
                Total range: {this.props.totalRange.first} {this.props.totalRange.last} ns
                Total duration: {totalDuration.value} {totalDuration.unit} <br />
                Duration: <input name={this.durationCtrlName} type="text" onChange={this.handleChange} value={this.state.duration} />
                Stop timestamp: <input name={this.stopTimestampCtrlName} type="text" onChange={this.handleChange} value={this.state.stopTimestamp} />
                <button onClick={this.changeRange}>Set range</button>
                <button onClick={this.changeRangeToMinMax}>Set range to current min/max</button>
            </div>)
    }
}

TimeRangeSelector.propTypes = {
    onRangeChanged: PropTypes.func.isRequired
}
