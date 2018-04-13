import React from 'react'

export class XYGraph extends React.Component {
    componentDidMount() {
        this.updateCanvas();
    }

    componentWillUpdate(nextState, nextProps) {
        this.updateCanvas();
    }

    _calculateYValue(yValue, ratio, margin) {
        return this.props.height - (yValue - this.props.data.minValue) * ratio - margin;
    }

    updateCanvas() {
        const ctx = this.refs.canvas.getContext('2d');
        const margin = 30;

        ctx.fillStyle = this.props.bgColor;
        ctx.fillRect(0,0, this.props.properties.width, this.props.height);

        if (!this.props.data) {
            return;
        }

        if (this.props.data.dataPoints.length > 0) {
            ctx.strokeStyle = this.props.color;
            ctx.beginPath();
            const height = this.props.height - 2 * margin;
            const diff = this.props.data.maxValue == this.props.data.minValue ? 1 : this.props.data.maxValue - this.props.data.minValue;
            const ratio = height / diff;
            ctx.moveTo(this.props.data.dataPoints[0].x, this._calculateYValue(this.props.data.dataPoints[0].y, ratio, margin));
            this.props.data.dataPoints.forEach((element, index, array) => {
                ctx.lineTo(element.x, this._calculateYValue(element.y, ratio, margin));
            });
            ctx.stroke();
        }
        ctx.font = "15px Arial";
        ctx.fillStyle = "black";
        ctx.fillText(this.props.title + " Min: " + this.props.data.minValue + " Max: " + this.props.data.maxValue, 0,15); 
    }

    render() {
        return <canvas onWheel={this.props.onMouseWheel} ref="canvas" width={this.props.properties.width} height={this.props.height} />
    }
}

XYGraph.defaultProps = {
    height: 300,
    bgColor: "#FFFFFF",
    color: "#000000",
    axisColor: '#AA00FF',
    data: undefined,
    title: "unnamed graph",
    onMouseWheel: undefined
};