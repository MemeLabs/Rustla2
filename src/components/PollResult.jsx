import React from 'react';
import PropTypes from 'prop-types';
import { connect } from 'react-redux';
import idx from 'idx';

import { beginPollingPoll } from '../actions';
import MainLayout from './MainLayout';


const PollResultRow = ({ value, votes, percent }) => (
  <React.Fragment>
    <div className='poll-result-label-row'>
      <span className='poll-result-label'>{value}</span>
      <span className='poll-result-count'>
        {votes} {votes === 1 ? 'vote' : 'votes'}
      </span>
    </div>
    <div className='poll-result-chart-row'>
      <div className='poll-result-bar-container'>
        <div
          className='poll-result-bar'
          style={{'width': `${percent}%`}}
        />
      </div>
      <span className='poll-result-percent'>
        {Math.round(percent)}%
      </span>
    </div>
  </React.Fragment>
);

PollResultRow.propTypes = {
  value: PropTypes.string.isRequired,
};

const PollResultChart = ({ poll: { subject, options } }) => {
  const maxValue = Object.values(options)
    .reduce((maxValue, value) => Math.max(maxValue, value), 0);

  const optionRows = Object.entries(options)
    .sort((a, b) => b[1] - a[1])
    .map(([value, votes], i) => (
      <PollResultRow
        key={i}
        value={value}
        votes={votes}
        percent={!maxValue ? 0 : (votes / maxValue) * 100}
      />
    ));

  return (
    <div>
      <h1>{subject}</h1>
      {optionRows}
    </div>
  );
};

PollResultChart.propTypes = {
  poll: PropTypes.object.isRequired,
};

class PollResult extends React.Component {
  componentDidMount() {
    this.stopPolling = this.props.beginPollingPoll(this.props.id);
  }

  componentWillUnmount() {
    this.stopPolling();
  }

  render() {
    const { poll, history } = this.props;

    const pollChart = poll && !poll.loading
    ? <PollResultChart poll={poll} />
    : <h4>Loading...</h4>;

    return (
      <MainLayout history={history}>
        <div className='container'>
          {pollChart}
        </div>
      </MainLayout>
    );
  }
}

PollResult.propTypes = {
  id: PropTypes.string.isRequired,
  poll: PropTypes.object,
  history: PropTypes.object.isRequired,
  beginPollingPoll: PropTypes.func.isRequired,
};

function mapDispatchToProps(dispatch) {
  return {
    beginPollingPoll: function(id) {
      return dispatch(beginPollingPoll(id));
    },
  };
}

export default connect(
  (state, ownProps) => {
    const id = ownProps.match.params.poll;
    return {
      id,
      poll: idx(state, _ => _.polls[id]),
    };
  },
  mapDispatchToProps,
)(PollResult);
