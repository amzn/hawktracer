import React from 'react';
import PropTypes from 'prop-types';

export class ConnectionIndicator extends React.Component {
    defaultConnectionURI = 'ws://localhost:4001/';

    constructor(props) {
        super(props);

        this.state = {
            connectionURI: this.defaultConnectionURI
        }
        this.handleChange = this.handleChange.bind(this);
    }

    getConnectButtonText() {
        return this.isConnected() ? 'Disconnect' : 'Connect';
    }

    isConnected() {
        return this.props.status != 'notconnected';
    }

    handleChange(evt) {
        this.setState({
            connectionURI: evt.target.value
        });
    }

    render() {
        return (
            <div>
                URI: <input type="text" disabled={this.isConnected()} value={this.state.connectionURI} onChange={this.handleChange} />
                <button onClick={() => this.props.onStateChangeRequest(this.state.connectionURI)}>
                    {this.getConnectButtonText()}
                </button>
            </div>)
    }
}

ConnectionIndicator.propTypes = {
    onStateChangeRequest: PropTypes.func.isRequired
}

ConnectionIndicator.defaultProps = {
    status: 'notconnected'
}